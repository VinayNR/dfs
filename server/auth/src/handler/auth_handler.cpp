#include "auth_handler.h"
#include "../net/sockets.h"
#include "../utils/utils.h"

#include <sstream>

// empty constructor
AuthServiceHandler::AuthServiceHandler() {}

AuthServiceHandler* AuthServiceHandler::getInstance() {
    static AuthServiceHandler instance;
    return &instance;
}

AuthRequest* AuthServiceHandler::readRequest(int sockfd) {
    std::cout << std::endl << " ---------- Inside read request ---------- " << std::endl;
    char *buffer = nullptr;
    int bytes = NetSocket::receive(sockfd, buffer, MAX_BUFFER_LENGTH, 5);

    if (bytes <= 0) {
        return nullptr;
    }

    // deserialize the request
    AuthRequest *auth_request;
    auth_request = deserializeRequest(buffer);

    // clean up
    deleteAndNullifyPointer(buffer, true);

    std::cout << " ---------- Finished read request ---------- " << std::endl;
    return auth_request;
}

int AuthServiceHandler::writeResponse(int sockfd, AuthResponse* auth_response) {
    std::cout << std::endl << " ---------- Inside write response ---------- " << std::endl;
    std::cout << auth_response->response_code << ", " << auth_response->response_message << ", " << auth_response->access_token << std::endl;
    char *buffer = nullptr;
    size_t size = serializeResponse(auth_response, buffer);
    if (NetSocket::send(sockfd, buffer, size) == -1) {
        std::cerr << "Failed to send response" << std::endl;
        return -1;
    }
    std::cout << " ---------- Finished write response ---------- " << std::endl;
    return 0;
}
