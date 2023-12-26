#pragma once

#include <string>
#include <sstream>

#include "data.h"

struct FileResponse {
    std::string code;
    std::string message;
    std::vector<std::string> options;
    
    // optional data
    Data data;

    static FileResponse* deserialize(const char *buffer) {
        FileResponse* response = new FileResponse;

        std::stringstream ss(buffer);
        std::string response_line;

        // extract first response line
        getline(ss, response_line, '\r');
        {
            std::stringstream ss_1(response_line);
            ss_1 >> response->code;
            ss_1 >> response->message;
        }

        if (ss.peek() == '\n') ss.ignore();
        
        if (ss.eof()) return response;

        // options
        while (getline(ss, response_line, '\r')) {
            if (ss.peek() == '\n') ss.ignore();

            if (response_line.size() == 0) break;

            response->options.push_back(response_line);
        }

        if (ss.eof()) return response;

        // extract the body
        getline(ss, response_line, '\r');
        {
            std::stringstream ss_1(response_line);
            ss_1 >> response->data.size;
        }

        // dyamically allocate memory for data
        response->data.data = new char[response->data.size + 1];
        memset(response->data.data, 0, response->data.size + 1);

        memcpy(response->data.data, buffer + ss.tellg(), response->data.size);

        return response;
    }

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

    static FileResponse* createFileSuccessResponse() {
        return new FileResponse{"201", "Created"};
    }

    static FileResponse* createTokenExpiredResponse() {
        return new FileResponse{"401", "Token Expired"};
    }

    static FileResponse* createForbiddenResponse() {
        return new FileResponse{"403", "Forbidden"};
    }
};