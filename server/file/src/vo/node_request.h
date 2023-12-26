#pragma once

#include <string>
#include <sstream>

#include "../net/utils.h"

#include "node.h"

struct NodeRequest {
    std::string request_type;
    INode node;
    int heartbeat_interval;
    std::vector<std::string> blocks;

    static NodeRequest* deserialize(const char *buffer) {
        NodeRequest *request = new NodeRequest;

        std::stringstream ss(buffer);
        std::string request_line;

        // extract first request line
        getline(ss, request_line, '\r');
        {
            std::stringstream ss_1(request_line);
            ss_1 >> request->request_type;

            // read ip and port
            std::string domain;
            ss_1 >> domain;
            // split ip obtained into ip and port
            char *ip = nullptr, *port = nullptr;
            getHostAndPort(domain.c_str(), ip, port);

            request->node.ip = ip;
            request->node.port = port;

            ss_1 >> request->heartbeat_interval;
        }

        if (ss.peek() == '\n') {
            ss.ignore();
        }

        std::string block_ids;
        // extract second request line
        getline(ss, block_ids, '\r');
        {
            std::istringstream iss(block_ids);

            // Extract words from the string and store them in the vector
            do {
                std::string word;
                iss >> word;
                if (!word.empty()) {
                    request->blocks.push_back(word);
                }
            } while (iss);
        }
        return request;
    }
};