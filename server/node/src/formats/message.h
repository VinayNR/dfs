#pragma once

#include <string>

// Struct to identify the node
struct INode {
    std::string ip;
    std::string port;

    std::string getFullDomain() const {
        return ip + ":" + port;
    }

    INode(std::string i, std::string p) : ip(i), port(p) {}
};

struct NodeConnectRequest {
    std::string request_type;
    INode node;
    int heartbeat_interval;

    NodeConnectRequest(std::string r_type, INode i_node, int interval) : request_type(r_type), node(i_node), heartbeat_interval(interval) {}
};

struct LeaderResponse {
    std::string code;
    std::string message;
};

LeaderResponse* deserializeLeaderResponse(const char *);
int serializeNodeConnectRequest(const NodeConnectRequest *, char *&);