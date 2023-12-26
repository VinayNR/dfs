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
};