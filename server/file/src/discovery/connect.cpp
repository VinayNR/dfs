#include "connect.h"
#include "../net/sockets.h"
#include "../net/utils.h"
#include "../utils/utils.h"

#include <sstream>

/*
* Serialization and Deserialization
*/
int serializeLeaderResponse(const LeaderResponse *response, char *& buffer) {
    // get the estimated size
    size_t size = 0;

    size += (response->code.size() + 1 + response->message.size());

    // create a dynamic char buffer
    buffer = new char[size + 1];
    memset(buffer, 0, size + 1);

    // reset the size for copying
    size = 0;

    // copy the contents
    memcpy(buffer + size, response->code.c_str(), response->code.size()); // command
    size += response->code.size();
    
    memcpy(buffer + size, " ", 1); // space
    size += 1;

    memcpy(buffer + size, response->message.c_str(), response->message.size());
    size += response->message.size();

    return size;
}

NodeConnectRequest* deserializeNodeConnectRequest(const char *buffer) {
    NodeConnectRequest *request = new NodeConnectRequest;

    std::stringstream ss(buffer);
    ss >> request->request_type;

    // read ip and port
    std::string domain;
    ss >> domain;
    // split ip obtained into ip and port
    char *ip = nullptr, *port = nullptr;
    getHostAndPort(domain.c_str(), ip, port);

    request->node.ip = ip;
    request->node.port = port;

    ss >> request->heartbeat_interval;

    return request;
}

NodeConnectRequest* receiveHeartBeat(int sockfd, struct sockaddr *& remote_address) {
    socklen_t serverlen = sizeof(struct sockaddr);

    // create the packet buffer data that is filled by the network
    char *buffer = new char[4096];
    memset(buffer, 0, 4096);
    
    // Receive a packet
    int bytesReceived = recvfrom(sockfd, buffer, 4096, 0, remote_address, &serverlen);

    if (bytesReceived == -1)  {
        return nullptr;
    }

    // deserialize the buffer
    NodeConnectRequest *heart_beat = deserializeNodeConnectRequest(buffer);

    // clean up pointers
    deleteAndNullifyPointer(buffer, true);

    return heart_beat;
}

NodeConnectRequest* readNodeConnectRequest(int sockfd) {
    char *buffer = nullptr;
    int bytes = SocketOps::receive(sockfd, buffer, 4096, 5);

    if (bytes <= 0) {
        return nullptr;
    }

    // deserialize the request
    NodeConnectRequest *request = deserializeNodeConnectRequest(buffer);

    // clean up
    deleteAndNullifyPointer(buffer, true);

    return request;
}

int writeLeaderResponse(int sockfd, LeaderResponse *response) {
    char *buffer = nullptr;
    int exit_code = 0;

    // serialize the request
    size_t size = serializeLeaderResponse(response, buffer);
    if (SocketOps::send(sockfd, buffer, size) == -1) {
        std::cerr << "Failed to send response" << std::endl;
        exit_code = 1;
    }

    // clean up
    deleteAndNullifyPointer(buffer, true);

    return exit_code;
}