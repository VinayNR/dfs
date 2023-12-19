#include "message.h"

#include <sstream>

int serializeNodeConnectRequest(const NodeConnectRequest *request, char *& buffer) {
    // get the size
    size_t size = 0;
    size += (request->request_type.size() + 1 + request->node.getFullDomain().size() + 1 + std::to_string(request->heartbeat_interval).size());
    buffer = new char[size + 1];
    memset(buffer, 0, size + 1);

    // reset the size for copying
    size = 0;

    // copy the contents
    memcpy(buffer + size, request->request_type.c_str(), request->request_type.size());
    size += request->request_type.size();

    memcpy(buffer + size, " ", 1);
    size += 1;

    memcpy(buffer + size, request->node.getFullDomain().c_str(), request->node.getFullDomain().size());
    size += request->node.getFullDomain().size();

    memcpy(buffer + size, " ", 1);
    size += 1;

    memcpy(buffer + size, std::to_string(request->heartbeat_interval).c_str(), std::to_string(request->heartbeat_interval).size());
    size += std::to_string(request->heartbeat_interval).size();

    return size;
}

LeaderResponse* deserializeLeaderResponse(const char *buffer) {
    LeaderResponse *response = new LeaderResponse;

    std::stringstream ss(buffer);
    ss >> response->code;
    ss >> response->message;

    return response;
}