#pragma once

#include <string>
#include <sstream>

struct AuthRequest {
    std::string request_type;
    std::string username;
    std::string password;

    int serialize(char *& buffer) {
        std::string serialized_request;
        serialized_request += ("CONNECT " + username + ":" + password + "\r\n");
        size_t size = serialized_request.length();
        buffer = new char[size + 1];
        memset(buffer, 0, size + 1);
        memcpy(buffer, serialized_request.c_str(), size);

        return size;
    }
};