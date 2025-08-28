// NamedPipeClient.cpp
#include "NamedPipeClient.h"
#include <windows.h>
#include <memory>
#include <iostream>

NamedPipeClient::NamedPipeClient(const wchar_t* pipeName, unsigned int connectTimeoutMs)
    : pipeName_(pipeName), connectTimeoutMs_(connectTimeoutMs) {}

NamedPipeClient::~NamedPipeClient() {}

bool NamedPipeClient::SendRequest(const std::string& request, std::string& response) {
    const int maxAttempts = 5;
    int attempts = 0;
    HANDLE h = INVALID_HANDLE_VALUE;

    while (attempts++ < maxAttempts) {
        h = CreateFileW(pipeName_.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
        if (h != INVALID_HANDLE_VALUE) break;

        DWORD err = GetLastError();
        if (err == ERROR_PIPE_BUSY) {
            // wait a bit
            if (!WaitNamedPipeW(pipeName_.c_str(), connectTimeoutMs_)) {
                // timed out
                continue;
            }
        }
        else {
            // cannot open pipe (server not running or permission denied)
            return false;
        }
    }

    if (h == INVALID_HANDLE_VALUE) return false;

    // ensure newline termination
    std::string req = request;
    if (req.empty() || req.back() != '\n') req.push_back('\n');

    // try TransactNamedPipe (atomic write-then-read)
    const DWORD BUF = 16 * 1024;
    std::unique_ptr<char[]> inBuf(new char[BUF]);
    DWORD bytesRead = 0;
    BOOL ok = TransactNamedPipe(h, (LPVOID)req.data(), (DWORD)req.size(), inBuf.get(), BUF, &bytesRead, nullptr);
    if (ok) {
        response.assign(inBuf.get(), inBuf.get() + bytesRead);
        CloseHandle(h);
        return true;
    }
    else {
        DWORD err = GetLastError();
        // Fallback: separate WriteFile + ReadFile
        DWORD written = 0;
        if (!WriteFile(h, req.data(), (DWORD)req.size(), &written, nullptr)) {
            CloseHandle(h);
            return false;
        }

        // Read loop (handle ERROR_MORE_DATA)
        std::string resp;
        while (true) {
            DWORD read = 0;
            BOOL r = ReadFile(h, inBuf.get(), BUF, &read, nullptr);
            if (!r) {
                DWORD rerr = GetLastError();
                if (rerr == ERROR_MORE_DATA) {
                    if (read > 0) resp.append(inBuf.get(), inBuf.get() + read);
                    continue;
                }
                else {
                    CloseHandle(h);
                    return false;
                }
            }
            else {
                if (read > 0) resp.append(inBuf.get(), inBuf.get() + read);
                break;
            }
        }
        response = resp;
        CloseHandle(h);
        return true;
    }
}
