#pragma once
#include <string>

class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void LogInfo(const std::string& msg) = 0;
    virtual void LogError(const std::string& msg) = 0;
};
