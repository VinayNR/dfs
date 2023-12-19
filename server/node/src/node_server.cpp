#include "node_server.h"

#include "formats/message.h"
#include "net/utils.h"
#include "net/sockets.h"

NodeServer::NodeServer(const char *config_file_path) {
    // load the configs for the server
    _configs = NodeServerConfigs::loadConfigs(config_file_path);

    // get an instance of the node service handler
    _node_handler = NodeServiceHandler::getInstance();

    // get an instance of the file service handler
    _file_handler = FileHandler::getInstance();
}

void NodeServer::setupSocket() {
    // set up the network socket for listening to client requests
    _listen_socket_fd = setupListeningSocket(_configs.getPort(), SOCK_STREAM);

    // setup a UDP socket for sending heart beats
    _heart_beats_socket_fd = SocketOps::createSocket(SOCK_DGRAM);
    std::cout << "Heart Beats UDP Socket: " << _heart_beats_socket_fd << std::endl;
}

void NodeServer::createClientThreadPool(int num_threads) {
    // create a thread pool of worker threads for the File server
    _client_thread_pool.init(num_threads);

    // initialize the client requests queue
    _client_requests_queue = new Queue<int>;

    // Create the lambda function for the auth server worker's task
    auto client_handling_task = [this]() {
        this->processClientRequests();
    };

    _client_thread_pool.createWorkerThreads(client_handling_task);
}

int NodeServer::discover() {
    std::cout << " ------------- Connecting with discovery server -------------- " << std::endl;

    // get the discovery server address    
    if (_configs.getDiscoveryServer() == "") {
        std::cerr << "Discovery server address not found in configs" << std::endl;
        return 1;
    }

    _discovery_socket_fd = connectToServer(_configs.getDiscoveryServer().c_str(), std::to_string(_configs.getDiscoveryPort()).c_str());
    std::cout << "Discovery FD: " << _discovery_socket_fd << std::endl;

    // construct the request
    NodeConnectRequest *request = new NodeConnectRequest("CONNECT", INode("127.0.0.1", std::to_string(_configs.getPort())), _configs.getHeartBeatInterval());

    // send the request
    _node_handler->writeNodeConnectRequest(_discovery_socket_fd, request);

    // wait for the response
    LeaderResponse *response = _node_handler->readLeaderResponse(_discovery_socket_fd);
    if (response->code != "200") {
        std::cerr << "Failed during discovery" << std::endl;
        SocketOps::closeSocket(_discovery_socket_fd);
        return 1;
    }

    SocketOps::closeSocket(_discovery_socket_fd);
    return 0;
}

void NodeServer::processClientRequests() {
    std::cout << std::endl << " ------ Node Server Thread Pool: " << std::this_thread::get_id() << " ------ " << std::endl;

    Request *request;
    Response *response;

    while (true) {
        // take a connection from the queue
        int client_sockfd = _client_requests_queue->dequeue();
        
        // if the client socket is valid
        if (client_sockfd != -1) {
            std::cout << std::this_thread::get_id() << " obtained a client socket: " << client_sockfd << std::endl;

            // do something here
            request = _node_handler->readFileRequest(client_sockfd);

            // handle the request
            if (request->command.type == "put") {
                // write the file to disk
                _file_handler->putFile(request->command.option.c_str(), request->data.data, request->data.size);

                // send a repsonse back
                response = Response::createFileSuccessResponse();
                _node_handler->writeFileReceiptResponse(client_sockfd, response);
            }
            else if (request->command.type == "get") {

            }

            // clean up
            delete request;
            delete response;

            // close the socket after communication is complete
            close(client_sockfd);
        }
    }
}

struct sockaddr * NodeServer::constructDiscoveryServerAddress() {

    struct addrinfo hints;
    struct addrinfo *serverinfo;

    // set the hints needed
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    // get server address info structures
    std::cout << "Address of discovery keepalive server: " << _configs.getDiscoveryServer().c_str() << " and port: " << _configs.getKeepAlivePort() << std::endl;
    int status = getaddrinfo(_configs.getDiscoveryServer().c_str(), std::to_string(_configs.getKeepAlivePort()).c_str(), &hints, &serverinfo);
    if (status != 0) {
        std::cout << "Error with get addr info: " << gai_strerror(status) << std::endl;
        return nullptr;
    }

    return (struct sockaddr *)serverinfo->ai_addr;
}

void NodeServer::sendKeepaliveHeartBeats() {
    std::cout << " ----------- Sending Heart beats ---------- " << std::endl;

    struct sockaddr *discovery_server = constructDiscoveryServerAddress();
    if (discovery_server == nullptr) {
        std::cerr << "Failed to obtain address of the discovery server" << std::endl;
        return;
    }
    
    // construct the request and the message buffer
    NodeConnectRequest *heart_beats = new NodeConnectRequest("ALIVE", INode("127.0.0.1", std::to_string(_configs.getPort())), _configs.getHeartBeatInterval());
    
    char *buffer = nullptr;
    int exit_code = 0;

    // serialize the request
    size_t size = serializeNodeConnectRequest(heart_beats, buffer);

    // keep sending every interval
    while (true) {
        // send the request
        SocketOps::sendTo(_heart_beats_socket_fd, buffer, size, discovery_server);

        // Sleep for timeout seconds
        std::cout << "Sent a heart beat" << std::endl;
        std::cout << "Sleeping for: " << _configs.getHeartBeatInterval() << " seconds" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(_configs.getHeartBeatInterval()));
    }

    delete heart_beats;
}

void NodeServer::start() {
    std::cout << std::endl << " -------- Starting Node Server --------- " << std::endl;

    // create a separate thread to send heart beat keep alive messagees to the leader
    std::thread discovery_thread(&NodeServer::sendKeepaliveHeartBeats, this);

    // create a client thread pool
    createClientThreadPool(10);

    // store client address during incoming connect requests
    int client_sockfd;

    while (true) {
        client_sockfd = SocketOps::acceptOnSocket(_listen_socket_fd);

        // add the connection received to the queue
        _client_requests_queue->enqueue(client_sockfd);

        std::cout << "Added client connection to queue: " << client_sockfd << std::endl;
    }

    discovery_thread.join();
}

void NodeServer::stop() {
    std::cout << " -------- Stopping Node Server --------- " << std::endl;

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

    // create an instance of Node server
    NodeServer node_server(argv[1]);

    // set up the server sockets
    node_server.setupSocket();

    // discover the cluster leader
    node_server.discover();

    // start the Node server and listen indefinitely for incoming client connections
    node_server.start();

    // shutdown the Node server
    node_server.stop();

    return 0;
}