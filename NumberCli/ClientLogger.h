#pragma once
#include "../Common/ILogger.h"
#include <iostream>

class ClientLogger : public ILogger {
public:
    void LogInfo(const std::string& msg) override {
        std::cout << "[Client-INFO] " << msg << "\n";
    }
    void LogError(const std::string& msg) override {
        std::cerr << "[Client-ERROR] " << msg << "\n";
    }
};
