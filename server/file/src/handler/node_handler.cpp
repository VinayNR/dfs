#include "node_handler.h"
#include "../net/sockets.h"
#include "../utils/utils.h"

// default constructor
NodeHandler::NodeHandler() {}

NodeHandler* NodeHandler::getInstance() {
    static NodeHandler instance;
    return &instance;
}

NodeRequest* NodeHandler::readRequest(int sockfd) {
    char *buffer = nullptr;
    int bytes = SocketOps::receive(sockfd, buffer, MAX_BUFFER_LENGTH, 5);

    if (bytes <= 0) {
        return nullptr;
    }

    // deserialize the request
    NodeRequest *request = NodeRequest::deserialize(buffer);

    // clean up
    deleteAndNullifyPointer(buffer, true);

    return request;
}

int NodeHandler::writeResponse(int sockfd, LeaderResponse * response) {
    char *buffer = nullptr;
    int exit_code = 0;

    // serialize the request
    size_t size = response->serialize(buffer);
    if (SocketOps::send(sockfd, buffer, size) == -1) {
        std::cerr << "Failed to send response" << std::endl;
        exit_code = 1;
    }

    // clean up
    deleteAndNullifyPointer(buffer, true);

    return exit_code;
}