// NamedPipeServer.cpp
#include "NamedPipeServer.h"
#include "../Common/NumberProtocol.h"
#include <sddl.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <memory>

NamedPipeServer::NamedPipeServer(const wchar_t* pipeName, RequestHandler handler, size_t workerThreads)
    : pipeName_(pipeName), handler_(std::move(handler)), running_(false), pool_(workerThreads) {}

NamedPipeServer::~NamedPipeServer() {
    Stop();
}

bool NamedPipeServer::CreateEveryoneSA(SECURITY_ATTRIBUTES& outSA, PSECURITY_DESCRIPTOR& outSD) {
    LPCWSTR sddl = L"D:(A;;GA;;;WD)"; // adjust for production
    outSD = nullptr;
    if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(sddl, SDDL_REVISION_1, &outSD, nullptr)) {
        outSA.nLength = sizeof(outSA);
        outSA.bInheritHandle = FALSE;
        outSA.lpSecurityDescriptor = nullptr;
        return false;
    }
    outSA.nLength = sizeof(outSA);
    outSA.bInheritHandle = FALSE;
    outSA.lpSecurityDescriptor = outSD;
    return true;
}

void NamedPipeServer::Start() {
    bool expected = false;
    if (!running_.compare_exchange_strong(expected, true)) return;
    listenThread_ = std::thread(&NamedPipeServer::ListenLoop, this);
}

void NamedPipeServer::Stop() {
    bool expected = true;
    if (!running_.compare_exchange_strong(expected, false)) {
        // already stopped
        return;
    }

    // Wake the listener by connecting a short-lived client
    HANDLE hWake = CreateFileW(pipeName_.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
    if (hWake != INVALID_HANDLE_VALUE) CloseHandle(hWake);

    if (listenThread_.joinable()) listenThread_.join();

    // Stop accepting new tasks and join worker threads
    pool_.Stop();
}

void NamedPipeServer::ListenLoop() {
    SECURITY_ATTRIBUTES sa{};
    PSECURITY_DESCRIPTOR psd = nullptr;
    if (!CreateEveryoneSA(sa, psd)) {
        sa.lpSecurityDescriptor = nullptr; // fallback to default
    }

    const DWORD outBuf = 8192;
    const DWORD inBuf = 8192;
    const DWORD maxInstances = 64; // sane cap; change as appropriate

    while (running_) {
        HANDLE pipe = CreateNamedPipeW(
            pipeName_.c_str(),
            PIPE_ACCESS_DUPLEX, // synchronous blocking
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            maxInstances,
            outBuf, inBuf, 0,
            (sa.lpSecurityDescriptor ? &sa : nullptr)
        );

        if (pipe == INVALID_HANDLE_VALUE) {
            DWORD err = GetLastError();
            std::cerr << "CreateNamedPipeW failed: " << err << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            continue;
        }

        BOOL connected = ConnectNamedPipe(pipe, nullptr);
        if (!connected) {
            DWORD err = GetLastError();
            if (err == ERROR_PIPE_CONNECTED) {
                // client connected between CreateFile and ConnectNamedPipe
            }
            else {
                // If server is stopping, break
                CloseHandle(pipe);
                continue;
            }
        }

        // Submit to thread pool. The worker will call HandleClient and then close handle.
        bool enqueued = pool_.Enqueue([this, pipe]() {
            HandleClient(pipe);
            });

        if (!enqueued) {
            // Pool stopping; close pipe
            CloseHandle(pipe);
        }
    }

    if (psd) LocalFree(psd);
}

void NamedPipeServer::HandleClient(HANDLE hPipe) {
    const DWORD BUF = 8192;
    std::unique_ptr<char[]> buffer(new char[BUF]);
    std::string req;

    // Read whole message (loop for ERROR_MORE_DATA)
    while (true) {
        DWORD bytesRead = 0;
        BOOL ok = ReadFile(hPipe, buffer.get(), BUF, &bytesRead, nullptr);
        if (!ok) {
            DWORD err = GetLastError();
            if (err == ERROR_MORE_DATA) {
                req.append(buffer.get(), buffer.get() + bytesRead);
                continue;
            }
            else {
                // read error or client disconnected
                CloseHandle(hPipe);
                return;
            }
        }
        else {
            if (bytesRead > 0) req.append(buffer.get(), buffer.get() + bytesRead);
            break;
        }
    }

    // Trim
    req = Trim(req);

    // Dispatch safely
    std::string resp;
    try {
        resp = handler_(req);
    }
    catch (const std::exception& ex) {
        resp = std::string("Error: handler threw: ") + ex.what();
    }
    catch (...) {
        resp = "Error: handler threw unknown exception.";
    }

    // Ensure newline termination for clarity (optional)
    if (resp.empty() || resp.back() != '\n') resp.push_back('\n');

    // Write response (loop to ensure full write)
    const char* outPtr = resp.data();
    size_t remaining = resp.size();
    while (remaining > 0) {
        DWORD written = 0;
        BOOL ok = WriteFile(hPipe, outPtr, (DWORD)remaining, &written, nullptr);
        if (!ok) {
            // write error
            break;
        }
        remaining -= written;
        outPtr += written;
    }

    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
}
