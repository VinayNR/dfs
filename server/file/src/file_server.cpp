#include "file_server.h"
#include "crypto/jwt.h"
#include "utils/utils.h"

FileServer::FileServer(const char *config_file_path) {
    // load the configs
    _configs = FileServerConfigs::loadConfigs(config_file_path);

    // get an instance of the request response handler
    _req_res_handler = RequestResponseHandler::getInstance();

    // get an instance of the file metadata handler
    _metadata_handler = MetadataHandler::getInstance();
}

void FileServer::setupSocket() {
    // set up a TCP socket and bind to the port
    _listen_socket_fd = setupListeningSocket(_configs.getPort(), SOCK_STREAM);
}

void FileServer::createClientThreadPool(int num_threads) {
    // create a thread pool of worker threads for the File server
    _client_thread_pool.init(num_threads);

    // initialize the client requests queue
    _client_requests_queue = new Queue<int>;

    // Create the lambda function for the server's worker tasks
    auto client_handling_task = [this]() {
        this->processClientRequests();
    };

    _client_thread_pool.createWorkerThreads(client_handling_task);
}

void FileServer::startDiscovery() {
    _discovery_server.start(_configs.getDiscoveryPort(), _configs.getKeepAlivePort());
}


void FileServer::processClientRequests() {
    std::cout << std::endl << " ------ File Server Thread Pool: " << std::this_thread::get_id() << " ------ " << std::endl;

    Request *request;
    Response *response;

    while (true) {
        // take a connection from the queue
        int client_sockfd = _client_requests_queue->dequeue();
        
        // if the client socket is valid
        if (client_sockfd != -1) {
            std::cout << std::this_thread::get_id() << " obtained a client socket: " << client_sockfd << std::endl;

            // do something here
            request = _req_res_handler->readRequest(client_sockfd);

            // validate the request
            std::cout << "Access token: " << request->access_token << std::endl;
            if (verifyTokenSingature(request->access_token, _configs.getSecret())) {
                // extract the token's payload
                std::size_t firstDot = request->access_token.find('.');
                std::size_t lastDot = request->access_token.rfind('.');

                if (firstDot == std::string::npos || lastDot == std::string::npos || firstDot == lastDot) {
                    // Invalid token format
                    std::cerr << "Invalid token format" << std::endl;
                }
                else {
                    std::string encoded_payload = request->access_token.substr(firstDot + 1, lastDot - firstDot - 1);
                    TokenPayload payload = TokenPayload::deserialize(base64Decode(encoded_payload));

                    std::cout << "Payload params: " << payload.sub << " with exp: " << payload.exp << std::endl;
                    std::cout << "Curr time: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << std::endl;

                    // check if token is expired
                    auto currentTime = std::chrono::system_clock::now();
                    if (std::stoul(payload.exp) >= std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) {
                        std::cout << "Token not expired" << std::endl;

                        // handle the request
                        if (request->command.type == "put") {
                            // divide the file into blocks and get the block specifications
                            std::string unique_file_uuid = payload.sub + ":" + request->command.option;
                            std::vector<Block> file_blocks = _metadata_handler->calculateBlocks(unique_file_uuid, request->data.size, _configs.getMinChunkSizeBytes(), _configs.getMaxChunks());

                            // determine the location of the blocks
                            for (Block& block : file_blocks) {
                                // get the block digest
                                unsigned int block_digest = computeMD5(block.block_id);

                                // check the position on the keyring
                                INode node = _discovery_server.getKeyRing().getNodeForKey(block_digest);

                                // Modify the original Block object
                                block.location.ip = node.ip;
                                block.location.port = std::stoi(node.port);
                            }

                            // create the file metadata
                            FileMetadata file_metadata;
                            file_metadata.file_name = request->command.option;
                            file_metadata.owner = payload.sub;
                            file_metadata.unique_file_uuid = unique_file_uuid;
                            file_metadata.blocks = file_blocks;
                            file_metadata.num_blocks = file_blocks.size();

                            // add the file metadata to the cache
                            _metadata_cache.set(unique_file_uuid, file_metadata);

                            // respond back to the client
                            response = new Response;
                            response->code = "200";
                            response->message = "OK";
                            for (Block block: file_blocks) {
                                response->options.push_back(block.getBlockAsString());
                            }
                        }
                        else if (request->command.type == "get") {

                        }

                        // write the response
                        _req_res_handler->writeResponse(client_sockfd, response);
                    }
                    else {
                        std::cout << "Token expired" << std::endl;
                        response = Response::createTokenExpiredResponse();
                        _req_res_handler->writeResponse(client_sockfd, response);
                    }
                }
            }
            else {
                std::cout << "Token is invalid" << std::endl;
                response = Response::createForbiddenResponse();
                _req_res_handler->writeResponse(client_sockfd, response);
            }

            // close the socket after communication is complete
            SocketOps::closeSocket(client_sockfd);

            delete request;
            delete response;
        }
    }
}

void FileServer::start() {
    std::cout << std::endl << " -------- Starting File Server --------- " << std::endl;

    // start file server discovery thread
    std::thread discoveryThread(&FileServer::startDiscovery, this);

    // create a thread pool for handling client requests
    createClientThreadPool(20);

    // store client address during incoming connect requests
    int client_sockfd;

    while (true) {
        client_sockfd = SocketOps::acceptOnSocket(_listen_socket_fd);

        // add the connection received to the queue
        _client_requests_queue->enqueue(client_sockfd);

        std::cout << "Added client connection to queue: " << client_sockfd << std::endl;
    }

    discoveryThread.join();
}

void FileServer::stop() {
    std::cout << " -------- Stopping File Server --------- " << std::endl;

    // wait for the worker threads to complete
    _client_thread_pool.joinWorkerThreads();

    // close the TCP socket
    SocketOps::closeSocket(_listen_socket_fd);
}

int main(int argc, char *argv[]) {
    // server command line arguments
    if (argc != 2) {
        std::cerr << "Incorrect number of arguments. Usage: <executable> <config_path>" << std::endl;
        exit(1);
    }

    // create an instance of File server
    FileServer file_server(argv[1]);

    // set up the server sockets
    file_server.setupSocket();

    // start the File server and listen indefinitely for incoming client connections
    file_server.start();

    // shutdown the Auth server
    file_server.stop();

    return 0;
}