// NamedPipeServer.h
#pragma once
#include "./../Common/IIpcServer.h"
#include <windows.h>
#include <string>
#include <atomic>
#include <thread>
#include <memory>
#include "ThreadPool.h"

class NamedPipeServer : public IIpcServer {
public:
    NamedPipeServer(const wchar_t* pipeName, RequestHandler handler, size_t workerThreads = 4);
    ~NamedPipeServer() override;

    void Start() override;
    void Stop() override;

private:
    void ListenLoop();
    void HandleClient(HANDLE hPipe);

    bool CreateEveryoneSA(SECURITY_ATTRIBUTES& outSA, PSECURITY_DESCRIPTOR& outSD);

    std::wstring pipeName_;
    RequestHandler handler_;
    std::atomic<bool> running_;
    std::thread listenThread_;
    ThreadPool pool_;
};
