#pragma once

#include <string>
#include <sstream>


struct LeaderResponse {
    std::string code;
    std::string message;

    int serialize(char *& buffer) {
        // get the estimated size
        size_t size = 0;

        size += (code.size() + 1 + message.size());

        // create a dynamic char buffer
        buffer = new char[size + 1];
        memset(buffer, 0, size + 1);

        // reset the size for copying
        size = 0;

        // copy the contents
        memcpy(buffer + size, code.c_str(), code.size()); // command
        size += code.size();
        
        memcpy(buffer + size, " ", 1); // space
        size += 1;

        memcpy(buffer + size, message.c_str(), message.size());
        size += message.size();

        return size;
    }
};