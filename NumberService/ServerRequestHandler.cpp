// ServerRequestHandler.cpp
#include "ServerRequestHandler.h"
#include <sstream>
#include <string>
#include <algorithm>
#include "../Common/NumberProtocol.h"

ServerRequestHandler::ServerRequestHandler(NumberStore& store) : store_(store) {}

std::string ServerRequestHandler::HandleRequest(const std::string& request) {
    std::istringstream iss(Trim(request));
    std::string cmd;
    iss >> cmd;
    if (cmd.empty()) return "Error: Empty command.\n";

    if (cmd == "insert") {
        int num; if (!(iss >> num)) return "Error: missing number.\n";
        if (num <= 0) return "Error: Only positive integers allowed.\n";
        bool inserted = store_.Insert(num);
        return inserted ? ("Inserted " + std::to_string(num) + "\n")
            : ("Error: Duplicate " + std::to_string(num) + "\n");
    }
    else if (cmd == "delete") {
        int num; if (!(iss >> num)) return "Error: missing number.\n";
        if (num <= 0) return "Error: Only positive integers allowed.\n";
        bool removed = store_.Remove(num);
        return removed ? ("Deleted " + std::to_string(num) + "\n")
            : ("Error: Not found " + std::to_string(num) + "\n");
    }
    else if (cmd == "print") {
        return store_.PrintAll();
    }
    else if (cmd == "delete_all") {
        store_.Clear();
        return "All numbers deleted.\n";
    }
    else if (cmd == "find") {
        int num; if (!(iss >> num)) return "Error: missing number.\n";
        long long ts;
        return store_.TryGetValue(num, ts) ? ("Found " + std::to_string(num) + ", Timestamp: " + std::to_string(ts) + "\n")
            : ("Not found " + std::to_string(num) + "\n");
    }

    return "Error: Unknown command.\n";
}
