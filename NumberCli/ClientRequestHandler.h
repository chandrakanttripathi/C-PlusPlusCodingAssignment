// ClientRequestHandler.h
#pragma once
#include "../Common/IRequestHandler.h"
#include "NamedPipeClient.h"
#include <iostream>

class ClientRequestHandler : public IRequestHandler {
public:
    explicit ClientRequestHandler(NamedPipeClient& client);
    std::string HandleRequest(const std::string& request) override;
    void RunCliLoop();

private:
    NamedPipeClient& client_;
};
