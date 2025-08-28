#pragma once
#include <string>

// This interface can be extended for any kind of Request Handler
class IRequestHandler {
public:
    virtual ~IRequestHandler() = default;
    virtual std::string HandleRequest(const std::string& request) = 0;
};
