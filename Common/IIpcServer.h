// IIpcServer.h
#pragma once
#include <string>
#include <functional>

// This will be used as a top level interface, so that it can be extended
//  to any IPC Mechanism
class IIpcServer {
public:
    using RequestHandler = std::function<std::string(const std::string&)>;

    virtual ~IIpcServer() = default;
    virtual void Start() = 0;
    virtual void Stop() = 0;
};