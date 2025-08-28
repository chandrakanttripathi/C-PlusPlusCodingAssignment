#pragma once
#include "../Common/ILogger.h"
#include <iostream>

class ServiceLogger : public ILogger {
public:
    void LogInfo(const std::string& msg) override {
        std::cout << "[Service-INFO] " << msg << "\n";
    }
    void LogError(const std::string& msg) override {
        std::cerr << "[Service-ERROR] " << msg << "\n";
    }
};
