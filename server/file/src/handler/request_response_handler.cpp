#include "request_response_handler.h"
#include "../net/sockets.h"
#include "../utils/utils.h"

// default constructor
RequestResponseHandler::RequestResponseHandler() {}

RequestResponseHandler* RequestResponseHandler::getInstance() {
    static RequestResponseHandler instance;
    return &instance;
}

Request* RequestResponseHandler::readRequest(int sockfd) {
    char *buffer = nullptr;
    int bytes = SocketOps::receive(sockfd, buffer, MAX_BUFFER_LENGTH, 5);

    if (bytes <= 0) {
        return nullptr;
    }

    // deserialize the request
    Request *request = Request::deserialize(buffer);

    // clean up
    deleteAndNullifyPointer(buffer, true);

    return request;
}

int RequestResponseHandler::writeResponse(int sockfd, Response * response) {
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