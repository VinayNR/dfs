#include "auth_handler.h"

#include "../net/sockets.h"
#include "../utils/utils.h"

#include <sstream>

// default constructor
AuthServiceHandler::AuthServiceHandler() {}

AuthServiceHandler* AuthServiceHandler::getInstance() {
    static AuthServiceHandler instance;
    return &instance;
}

AuthResponse* AuthServiceHandler::readResponse(int sockfd) {
    char *buffer = nullptr;
    int bytes = SocketOps::receive(sockfd, buffer, MAX_BUFFER_LENGTH, 5);

    if (bytes <= 0) {
        return nullptr;
    }

    // deserialize the request
    AuthResponse *auth_response = AuthResponse::deserialize(buffer);

    std::cout << "Finished deserialization of auth" << std::endl;

    // clean up
    deleteAndNullifyPointer(buffer, true);

    return auth_response;
}

int AuthServiceHandler::writeRequest(int sockfd, AuthRequest *auth_request) {
    char *buffer = nullptr;
    int exit_code = 0;

    // serialize the request
    size_t size = auth_request->serialize(buffer);
    if (SocketOps::send(sockfd, buffer, size) == -1) {
        std::cerr << "Failed to send request" << std::endl;
        exit_code = 1;
    }

    // clean up
    deleteAndNullifyPointer(buffer, true);

    return exit_code;
}