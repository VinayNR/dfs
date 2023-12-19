#include "client_handler.h"

#include "../net/sockets.h"
#include "../utils/utils.h"

#include <sstream>

// default constructor
ClientHandler::ClientHandler() {
    _file_handler = FileHandler::getInstance();
}

ClientHandler* ClientHandler::getInstance() {
    static ClientHandler instance;
    return &instance;
}

Response* ClientHandler::readResponse(int sockfd) {
    char *buffer = nullptr;
    int bytes = SocketOps::receive(sockfd, buffer, MAX_BUFFER_LENGTH, 5);

    if (bytes <= 0) {
        return nullptr;
    }

    // deserialize the response
    Response *response = Response::deserialize(buffer);

    // clean up
    deleteAndNullifyPointer(buffer, true);

    return response;
}

int ClientHandler::writeRequest(int sockfd, Request *request) {
    char *buffer = nullptr;
    int exit_code = 0;

    // serialize the request
    size_t size = request->serialize(buffer);
    if (SocketOps::send(sockfd, buffer, size) == -1) {
        std::cerr << "Failed to send request" << std::endl;
        exit_code = -1;
    }

    // clean up
    deleteAndNullifyPointer(buffer, true);

    return exit_code;
}

int ClientHandler::handlePut(const char *filename, char *& file_contents) {
    int file_size = _file_handler->getFile(filename, file_contents);
    return file_size;
}

Request * ClientHandler::constructRequest(Command command, Data data, const std::string &access_token) {
    Request *request = new Request;
    request->command = command;
    request->data = data;
    request->access_token = access_token;

    return request;
}