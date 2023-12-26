#pragma once

#include <string>
#include <sstream>

#include "command.h"
#include "data.h"

struct FileRequest {
    Command command;
    std::string access_token;

    // optional data
    Data data;

    static FileRequest* deserialize(const char *buffer) {
        FileRequest* request = new FileRequest;

        std::stringstream ss(buffer);
        std::string request_line;

        // extract first request line
        getline(ss, request_line, '\r');
        {
            std::stringstream ss_1(request_line);
            ss_1 >> request->command.type;
            ss_1 >> request->command.option;
        }

        if (ss.peek() == '\n') ss.ignore();

        // extract second request line
        getline(ss, request_line, '\r');
        {
            std::stringstream ss_1(request_line);
            ss_1 >> request->access_token;
        }

        if (ss.peek() == '\n') ss.ignore();

        if (ss.eof()) return request;

        // extract third request line: <must be empty>
        getline(ss, request_line, '\r');

        if (ss.peek() == '\n') ss.ignore();

        // extract the body
        getline(ss, request_line, '\r');
        {
            std::stringstream ss_1(request_line);
            ss_1 >> request->data.size;
        }

        if (ss.peek() == '\n') ss.ignore();

        // dyamically allocate memory for data
        request->data.data = new char[request->data.size + 1];
        memset(request->data.data, 0, request->data.size + 1);

        memcpy(request->data.data, buffer + ss.tellg(), request->data.size);

        return request;
    }
};