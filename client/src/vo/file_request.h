#pragma once

#include <string>

#include "command.h"
#include "data.h"

struct FileRequest {
    Command command;
    std::string access_token;

    // optional data
    Data data;

    size_t serialize(char *& buffer) {
        // estimate the size of the request
        size_t size = 0;

        // first line
        size += (command.type.size() + 1 + command.option.size() + 2);  // <type> <option>\r\n

        // second line
        size += (access_token.size() + 2); // <token>\r\n

        // third line
        if (data.size > 0) {
            size += (2 + std::to_string(data.size).size() + 2); // \r\n<size>\r\n
            size += data.size;
        }
        
        // create the buffer
        buffer = new char[size + 1];
        memset(buffer, 0, size + 1);

        // copy the contents
        size = 0;

        // first line
        memcpy(buffer + size, command.type.c_str(), command.type.size());
        size += (command.type.size());
        memcpy(buffer + size, " ", 1);
        size += 1;
        memcpy(buffer + size, command.option.c_str(), command.option.size());
        size += (command.option.size());
        memcpy(buffer + size, "\r\n", 2);
        size += 2;

        // second line
        memcpy(buffer + size, access_token.c_str(), access_token.size());
        size += access_token.size();
        memcpy(buffer + size, "\r\n", 2);
        size += 2;

        // third line
        if (data.size > 0) {
            memcpy(buffer + size, "\r\n", 2);
            size += 2;
            memcpy(buffer + size, std::to_string(data.size).c_str(), std::to_string(data.size).size());
            size += (std::to_string(data.size).size());
            memcpy(buffer + size, "\r\n", 2);
            size += 2;
            // check if data is not null
            if (data.data) {
                memcpy(buffer + size, data.data, data.size);
                size += data.size;
            }
        }

        return size;
    }
};