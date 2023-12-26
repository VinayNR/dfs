#pragma once

#include <string>

struct INode {
    std::string ip;
    std::string port;

    std::string getFullDomain() {
        return ip + ":" + port;
    }

    unsigned int node_digest;
};