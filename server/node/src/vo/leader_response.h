#pragma once

#include <string>
#include <sstream>

struct LeaderResponse {
    std::string code;
    std::string message;

    static LeaderResponse* deserialize(const char *buffer) {
        LeaderResponse *response = new LeaderResponse;

        std::stringstream ss(buffer);
        ss >> response->code;
        ss >> response->message;

        return response;
    }
};