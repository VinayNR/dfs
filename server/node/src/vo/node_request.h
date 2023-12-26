#pragma once

#include "node.h"
#include "../cache/cache.h"

#include <string>
#include <sstream>

struct NodeRequest {
    std::string request_type;
    INode node;
    int heartbeat_interval;

    Cache<std::string, bool> *block_cache;

    NodeRequest(std::string r_type, INode i_node, int interval, Cache<std::string, bool> *c) : request_type(r_type), node(i_node), heartbeat_interval(interval), block_cache(c) {}

    int serialize(char *& buffer) {
        // get the size
        size_t size = 0;
        size += (request_type.size() + 1 + node.getFullDomain().size() + 1 + std::to_string(heartbeat_interval).size() + 2);
        
        for (auto& pair : block_cache->getCacheStore()) {
            size += pair.first.size();
            size += 1; // for the space
        }

        buffer = new char[size + 1];
        memset(buffer, 0, size + 1);

        // reset the size for copying
        size = 0;

        // copy the contents
        memcpy(buffer + size, request_type.c_str(), request_type.size());
        size += request_type.size();

        memcpy(buffer + size, " ", 1);
        size += 1;

        memcpy(buffer + size, node.getFullDomain().c_str(), node.getFullDomain().size());
        size += node.getFullDomain().size();

        memcpy(buffer + size, " ", 1);
        size += 1;

        memcpy(buffer + size, std::to_string(heartbeat_interval).c_str(), std::to_string(heartbeat_interval).size());
        size += std::to_string(heartbeat_interval).size();

        memcpy(buffer + size, "\r\n", 2);
        size += 2;

        for (const auto &pair: block_cache->getCacheStore()) {
            memcpy(buffer + size, pair.first.c_str(), pair.first.size());
            size += pair.first.size();

            memcpy(buffer + size, " ", 1);
            size += 1;
        }

        return size;
    }
};