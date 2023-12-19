#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "data.h"

struct Response {
    std::string code;
    std::string message;
    std::vector<std::string> options;
    
    // optional data
    Data data;

    int serialize(char *& buffer) {
        // get the estimated size
        size_t size = 0;

        // first line
        size += (code.size() + 1 + message.size() + 2); // <code> <message>\r\n

        // options
        for (std::string option: options) {
            size += option.size() + 2; //<option>\r\n
        }

        if (data.size > 0) {
            size += 2; // \r\n
            size += std::to_string(data.size).size();
            size += 2; // \r\n
            size += data.size;
        }

        // create a dynamic char buffer
        buffer = new char[size + 1];
        memset(buffer, 0, size + 1);

        // reset the size for copying
        size = 0;

        // copy the contents

        // first line
        memcpy(buffer + size, code.c_str(), code.size());
        size += code.size();
        memcpy(buffer + size, " ", 1);
        size += 1;
        memcpy(buffer + size, message.c_str(), message.size());
        size += message.size();
        memcpy(buffer + size, "\r\n", 2);
        size += 2;

        // options
        for (std::string option: options) {
            // std::cout << "option during serializer: " << option << std::endl;
            memcpy(buffer + size, option.c_str(), option.size());
            size += option.size();
            memcpy(buffer + size, "\r\n", 2);
            size += 2;
        }

        // check if there is a file content to serialize
        if (data.size > 0) {
            memcpy(buffer + size, "\r\n", 2);
            size += 2;

            memcpy(buffer + size, std::to_string(data.size).c_str(), std::to_string(data.size).size());
            size += std::to_string(data.size).size();

            memcpy(buffer + size, "\r\n", 2);
            size += 2;

            memcpy(buffer + size, data.data, data.size);
            size += data.size;
        }

        return size;
    }

    static Response* createTokenExpiredResponse() {
        return new Response{"401", "Token Expired"};
    }

    static Response* createForbiddenResponse() {
        return new Response{"403", "Forbidden"};
    }
};