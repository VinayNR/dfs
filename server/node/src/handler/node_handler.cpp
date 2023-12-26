#include "node_handler.h"
#include "../net/sockets.h"
#include "../utils/utils.h"

// default constructor
NodeServiceHandler::NodeServiceHandler() {}

NodeServiceHandler* NodeServiceHandler::getInstance() {
    static NodeServiceHandler instance;
    return &instance;
}

int NodeServiceHandler::writeNodeRequest(int sockfd, NodeRequest *request) {
    char *buffer = nullptr;
    int exit_code = 0;

    // serialize the request
    size_t size = request->serialize(buffer);
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
    LeaderResponse *response = LeaderResponse::deserialize(buffer);

    // clean up
    deleteAndNullifyPointer(buffer, true);

    return response;
}

FileRequest * NodeServiceHandler::readFileRequest(int sockfd) {
    char *buffer = nullptr;
    std::vector<char> v;
    int buffer_size = 0;

    std::size_t content_length = 0, total_length = 0, header_size = 0;

    do {
        
        if ((buffer_size = SocketOps::receive(sockfd, buffer, 4096, 5)) <= 0) break;

        if (content_length == 0) {
            // get the position of \r\n\r\n
            const char* doubleCRLF = strstr(buffer, "\r\n\r\n");
            if (doubleCRLF != nullptr) {
                header_size = doubleCRLF - buffer;
                std::cout << "Header Size: " << header_size << std::endl;
            }

            // get content length of the data
            doubleCRLF += 4;
            content_length = atoi(doubleCRLF);

            if (content_length == 0) {
                v.insert(v.end(), buffer, buffer + buffer_size);
                total_length = buffer_size;
                break;
            }
            std::cout << "Content Size: " << content_length << std::endl;
        }

        // copy buffer into data
        v.insert(v.end(), buffer, buffer + buffer_size);
        total_length += buffer_size;

        // check if everything is read from the socket
        if (content_length + header_size <= total_length) {
            break;
        }

        // clear the buffer
        deleteAndNullifyPointer(buffer, true);

    } while(true);

    // clear the buffer
    deleteAndNullifyPointer(buffer, true);

    // deserialize the request
    FileRequest *request = FileRequest::deserialize(v.data());

    if (total_length > 0) {
        std::cout << "Length of data read: " << total_length << std::endl;
        std::cout << " -------- Finished Read Data ------- " << std::endl;
    }

    return request;
}

int NodeServiceHandler::writeFileReceiptResponse(int sockfd, FileResponse *response) {
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