#pragma once

#include <string>

#include "node.h"

struct NodeConnectRequest {
    std::string request_type;
    INode node;
    int heartbeat_interval;
};

struct LeaderResponse {
    std::string code;
    std::string message;
};

int serializeLeaderResponse(const LeaderResponse *, char *&);
NodeConnectRequest* deserializeNodeConnectRequest(const char *);

NodeConnectRequest* readNodeConnectRequest(int);
int writeLeaderResponse(int, LeaderResponse *);

NodeConnectRequest* receiveHeartBeat(int, struct sockaddr *&);