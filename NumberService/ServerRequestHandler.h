// ServerRequestHandler.h
#pragma once
#include "../Common/IRequestHandler.h"
#include "NumberStore.h"

class ServerRequestHandler : public IRequestHandler {
public:
    explicit ServerRequestHandler(NumberStore& store);
    std::string HandleRequest(const std::string& request) override;

private:
    NumberStore& store_;
};
