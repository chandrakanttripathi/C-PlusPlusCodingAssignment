// NamedPipeClient.h
#pragma once
#include "IIpcClient.h"
#include <string>

class NamedPipeClient : public IIpcClient {
public:
    explicit NamedPipeClient(const wchar_t* pipeName, unsigned int connectTimeoutMs = 2000);
    ~NamedPipeClient() override;

    bool SendRequest(const std::string& request, std::string& response) override;

private:
    std::wstring pipeName_;
    unsigned int connectTimeoutMs_;
};
