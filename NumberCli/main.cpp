// main_client.cpp
#include "NamedPipeClient.h"
#include "ClientRequestHandler.h"
#include "NumberProtocol.h"
#include <iostream>

int main() {
    std::cout << "Connecting to NumberService at " << "NumberServicePipe" << "...\n";
    NamedPipeClient client(kPipeName);
    ClientRequestHandler cliHandler(client);
    cliHandler.RunCliLoop();
    return 0;
}
