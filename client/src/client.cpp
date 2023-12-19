#include <iostream>
#include <algorithm>
#include <thread>

#include "client.h"
#include "net/sockets.h"
#include "net/utils.h"
#include "utils/utils.h"
#include "vo/blocks.h"


DFSClient::DFSClient(char *filename) {
    // read the client configs
    _configs = ClientConfigs::loadConfigs(filename);

    // get an instance of the auth handler
    _auth_handler = AuthServiceHandler::getInstance();

    // get an instance of the client handler
    _client_handler = ClientHandler::getInstance();

    // get an instance of the command handler
    _command_handler = CommandHandler::getInstance();

    // connect to the server
    std::cout << "Connecting to the authorization server and authenticating..." << std::endl;

    // authenticate with the server
    if (authenticate() == 1) {
        std::cerr << "Exiting the client" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void DFSClient::start() {
    std::cout << " ------------- DFS Client -------------- " << std::endl;

    // get the nameserver address
    if (_configs.getServers().count("file") <= 0) {
        std::cerr << "Error finding the address of the file server" << std::endl;
        exit(EXIT_FAILURE);
    }

    char *file__server_host = nullptr, *file_server_port = nullptr;
    // get the host and port number of the auth server
    getHostAndPort(_configs.getServers()["file"].c_str(), file__server_host, file_server_port);

    std::string command_str;
    Request *request;
    Response *response;

    // client main loop
    while (true) {
        std::cout << "> ";
        getline(std::cin, command_str);

        // validate the command
        if (!_command_handler->validateCommand(command_str)) {
            std::cerr << "Command not valid" << std::endl;
            continue;
        }

        // connect to the server
        int sockfd = connectToServer(file__server_host, file_server_port);

        // construct the command
        Command command = _command_handler->constructCommand(command_str);

        if (command.type == "put") {
            // read the file
            char *file_contents = nullptr;
            size_t file_size = _client_handler->handlePut(command.option.c_str(), file_contents);

            // data object for sending request to metadata server
            Data data{nullptr, file_size};

            // construct the request
            request = _client_handler->constructRequest(command, data, _access_token);

            // write the request to the metadata server
            if (_client_handler->writeRequest(sockfd, request) == -1) {
                std::cerr << "Failed to write the request" << std::endl;
                delete request;
                close(sockfd);
                continue;
            }
            delete request;

            // wait for a response
            response = _client_handler->readResponse(sockfd);
            if (response != nullptr) {
                std::cout << "Response - " << response->code << " : " << response->message << std::endl;
            }

            // process the response from metadata server and add the blocks to the locations
            Block block;
            std::vector<Block> blocks;
            
            for (std::string option: response->options) {
                std::stringstream ss(option);
                ss >> block.block_id >> block.start >> block.end >> block.location.ip >> block.location.port;
                blocks.push_back(block);
            }

            delete response;

            // write the blocks to the node servers
            writeBlocks(file_contents, blocks);
        }
        else if (command.type == "get") {
            
        }
        else if (command.type == "list") {

        }
        close(sockfd);
    }
}

void DFSClient::writeBlock(const char *file_contents, Block block) {
    Command command {"put", block.block_id};

    char *block_contents = new char[block.end - block.start + 2];
    memset(block_contents, 0, block.end - block.start + 2);
    memcpy(block_contents, file_contents + block.start, block.end - block.start + 1);

    Data data{block_contents, block.end - block.start + 1};

    Request *request = _client_handler->constructRequest(command, data, _access_token);

    // Connect to the server
    int sockfd = connectToServer(block.location.ip.c_str(), std::to_string(block.location.port).c_str());

    // send the request
    _client_handler->writeRequest(sockfd, request);

    // wait for the response
    Response *response = _client_handler->readResponse(sockfd);

    if (response->code == "201") {
        std::cout << "Block written successfully!" << std::endl;
    }

    delete request;
    delete response;
    delete[] block_contents;
    close(sockfd);
}

void DFSClient::writeBlocks(const char *file_contents, const std::vector<Block> &blocks) {
    std::vector<std::thread> threads;

    for (const Block &block : blocks) {
        threads.emplace_back(&DFSClient::writeBlock, this, file_contents, block);
    }

    // Join all the threads to wait for their completion
    for (auto &thread : threads) {
        thread.join();
    }
}

int DFSClient::authenticate() {
    std::cout << " ------------- Authenticating with server -------------- " << std::endl;

    // get the auth server address    
    if (_configs.getServers().count("auth") == 0) {
        std::cerr << "Authentication server address not found in client configs" << std::endl;
        return 1;
    }
    
    char *host = nullptr, *port = nullptr;

    // get the host and port number of the auth server
    getHostAndPort(_configs.getServers()["auth"].c_str(), host, port);

    int sockfd = connectToServer(host, port);

    // send the request
    AuthRequest *auth_request = new AuthRequest;
    auth_request->username = _configs.getUserName();
    auth_request->password = _configs.getPassword();
    auth_request->request_type = "CONNECT";

    // write the request
    _auth_handler->writeRequest(sockfd, auth_request);

    // get the response from the auth server
    AuthResponse *auth_response = _auth_handler->readResponse(sockfd);
    if (auth_response->response_code == "401") {
        std::cerr << "Unauthorized" << std::endl;
        return 1;
    }

    // set the access token
    _access_token = auth_response->access_token;
    std::cout << "Authentication Access Token : " << _access_token << std::endl;

    // clean up
    delete auth_request;
    delete auth_response;

    close(sockfd);
    
    return 0;
}

int main(int argc, char * argv[]) {

    // assert the usage of the client program
    if (argc != 2) {
        std::cerr << "Usage error: <executable> <config_path>" << std::endl;
        exit(EXIT_FAILURE);
    }

    // create a DFS Client
    DFSClient client(argv[1]);

    // main client loop
    client.start();

    return 0;
}