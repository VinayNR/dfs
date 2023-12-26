#include "file_handler.h"

#include "../net/sockets.h"
#include "../utils/utils.h"

#include <dirent.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

// default constructor
FileHandler::FileHandler() {
}

FileHandler* FileHandler::getInstance() {
    static FileHandler instance;
    return &instance;
}

FileResponse* FileHandler::readResponse(int sockfd) {
    char *buffer = nullptr;
    int bytes = SocketOps::receive(sockfd, buffer, MAX_BUFFER_LENGTH, 5);

    if (bytes <= 0) {
        return nullptr;
    }

    // deserialize the response
    FileResponse *response = FileResponse::deserialize(buffer);

    // clean up
    deleteAndNullifyPointer(buffer, true);

    return response;
}

int FileHandler::writeRequest(int sockfd, FileRequest *request) {
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

int FileHandler::handlePut(const char *filename, char *& file_contents) {
    int file_size = getFile(filename, file_contents);
    return file_size;
}

FileRequest * FileHandler::constructRequest(Command command, Data data, const std::string &access_token) {
    FileRequest *request = new FileRequest;
    request->command = command;
    request->data = data;
    request->access_token = access_token;

    return request;
}

int FileHandler::getFile(const char * filename, char *& fileContents) {
    std::ifstream filestream (filename, std::ios::binary | std::ios::ate);
    if (!filestream.is_open()) {
        std::cerr << "Unable to open file" << std::endl;
        return -1;
    }

    int fileSize = filestream.tellg();
    filestream.seekg(0, std::ios::beg);

    fileContents = new char[fileSize + 1];
    memset(fileContents, 0, fileSize + 1);

    if (!filestream.read(fileContents, fileSize)) {
        std::cerr << "Unable to read file" << std::endl;
        return -1;
    }

    filestream.close();
    return fileSize;
}

int FileHandler::putFile(const char * filename, char * fileContents, int dataSize) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile.is_open()) {
        return -1;
    }

    // Write dataSize bytes from fileContents into the file
    outFile.write(fileContents, dataSize);

    if (outFile.fail()) {
        return -1;
    }

    outFile.close();
    return 0;
}