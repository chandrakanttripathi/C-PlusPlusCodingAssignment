// ClientRequestHandler.cpp
#include "ClientRequestHandler.h"
#include <limits>

ClientRequestHandler::ClientRequestHandler(NamedPipeClient& client) : client_(client) {}

std::string ClientRequestHandler::HandleRequest(const std::string& request) {
    std::string response;
    bool ok = client_.SendRequest(request, response);
    if (!ok) return "Error: failed to contact service (is it running?)\n";
    return response;
}

void ClientRequestHandler::RunCliLoop() {
    while (true) {
        std::cout << "\nChoose option:\n"
            << "1. Insert number\n"
            << "2. Delete number\n"
            << "3. Print all numbers\n"
            << "4. Delete all numbers\n"
            << "5. Find number\n"
            << "6. Exit\n"
            << "Choice: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input.\n";
            continue;
        }

        if (choice == 6) break;

        std::string req;
        if (choice == 1) {
            int num; std::cout << "Enter number: "; std::cin >> num;
            req = "insert " + std::to_string(num);
        }
        else if (choice == 2) {
            int num; std::cout << "Enter number: "; std::cin >> num;
            req = "delete " + std::to_string(num);
        }
        else if (choice == 3) req = "print";
        else if (choice == 4) req = "delete_all";
        else if (choice == 5) {
            int num; std::cout << "Enter number: "; std::cin >> num;
            req = "find " + std::to_string(num);
        }
        else {
            std::cout << "Invalid choice.\n";
            continue;
        }

        std::string resp = HandleRequest(req);
        std::cout << resp;
    }
}
