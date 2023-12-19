#include "node_handler.h"
#include "../net/sockets.h"
#include "../utils/utils.h"

// default constructor
NodeServiceHandler::NodeServiceHandler() {}

NodeServiceHandler* NodeServiceHandler::getInstance() {
    static NodeServiceHandler instance;
    return &instance;
}

int NodeServiceHandler::writeNodeConnectRequest(int sockfd, NodeConnectRequest *request) {
    char *buffer = nullptr;
    int exit_code = 0;

    // serialize the request
    size_t size = serializeNodeConnectRequest(request, buffer);
    if (SocketOps::send(sockfd, buffer, size) == -1) {
        std::cerr << "Failed to send response" << std::endl;
        exit_code = 1;
    }

    // clean up
    deleteAndNullifyPointer(buffer, true);

    return exit_code;
}

LeaderResponse* NodeServiceHandler::readLeaderResponse(int sockfd) {
    char *buffer = nullptr;
    int bytes = SocketOps::receive(sockfd, buffer, 4096, 5);

    if (bytes <= 0) {
        return nullptr;
    }

    // deserialize the request
    LeaderResponse *response = deserializeLeaderResponse(buffer);

    // clean up
    deleteAndNullifyPointer(buffer, true);

    return response;
}