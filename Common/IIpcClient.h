// IIpcClient.h
#pragma once
#include <string>

// This interface will be for client level
class IIpcClient {
public:
    virtual ~IIpcClient() = default;
    virtual bool SendRequest(const std::string& request, std::string& response) = 0;
};