#include "auth_server.h"
#include "crypto/jwt.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>

std::unordered_map<std::string, std::string> AuthServer::getUsers() {
    return _users;
}

AuthServer::AuthServer(const char *config_file_path) {

    // loading configuration files
    _configs = AuthConfigs::loadConfigs(config_file_path);

    // read the userdb file
    loadUserDB(_configs.getUserDB());

    // set up its network socket
    setupSocket(_configs.getPort());

    // get an instance of the auth request handler
    _auth_handler = AuthServiceHandler::getInstance();

    // create a thread pool of worker threads for the Auth server
    _client_thread_pool.init(10);

    // initialize the client requests queue
    _client_requests_queue = new Queue<int>;
    
    // Create the lambda function for the auth server worker's task
    auto client_handling_task = [this]() {
        this->processClientRequests();
    };

    _client_thread_pool.createWorkerThreads(client_handling_task);
}

void AuthServer::processClientRequests() {
    std::cout << std::endl << " ------ Auth Server Thread Pool: " << std::this_thread::get_id() << " ------ " << std::endl;

    while (true) {
        // take a connection from the queue
        int client_sockfd = _client_requests_queue->dequeue();
        
        // if the client socket is valid
        if (client_sockfd != -1) {
            std::cout << std::this_thread::get_id() << " obtained a client socket: " << client_sockfd << std::endl;

            // read the client request
            AuthRequest *auth_request = nullptr;
            auth_request = _auth_handler->readRequest(client_sockfd);

            if (auth_request == nullptr) continue;

            AuthResponse *auth_response = new AuthResponse;

            // check the database for the username and password
            std::cout << "Checking username: " << auth_request->username << " and password: " << auth_request->password << std::endl;
            if (_users.count(auth_request->username) == 0
            || _users[auth_request->username] != auth_request->password) {
                // unauthorized user
                std::cout << "User unauthorized... " << std::endl;
                auth_response->response_code = "401";
                auth_response->response_message = "Unauthorized";
            }
            else {
                // provide an access token for the user
                auth_response->response_code = "200";
                auth_response->response_message = "Authenticated";

                // create the jwt token using the private key and the payload
                // Create a stringstream to build the JSON string
                std::stringstream ss;

                // Get the current time point
                auto currentTime = std::chrono::system_clock::now();

                // Add 30 minutes to the current time, to establish validity of this token
                auto newTime = currentTime + std::chrono::minutes(30);

                // Convert the new time to milliseconds
                auto duration = newTime.time_since_epoch();
                auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

                TokenHeader header{"HS256", "JWT"};
                TokenPayload payload{auth_request->username, _configs.getName(), std::to_string(milliseconds)};

                auth_response->access_token = createJWTToken(header.serialize(), payload.serialize(), _configs.getSecret());
            }

            // send the response back
            _auth_handler->writeResponse(client_sockfd, auth_response);

            // close the socket after communication is complete
            close(client_sockfd);
        }
    }
}

int AuthServer::setupSocket(int port) {
    std::cout << std::endl << "---------- Setting up socket for the server ---------- " << std::endl;

    // create a tcp socket
    _listen_socket.createSocket();

    // display the socket
    std::cout << "Socket FD: " << _listen_socket.getSocketID() << std::endl;

    /*
    * setsockopt: Handy debugging trick that lets us rerun the server immediately after we kill it; 
    * otherwise we have to wait about 20 secs. 
    */
    int optval = 1;
    setsockopt(_listen_socket.getSocketID(), SOL_SOCKET, optval, (const void *)optval , sizeof(int));

    // bind the socket to the port
    _listen_socket.bindSocket(std::to_string(port).c_str());

    // listen on the socket for incoming connections
    _listen_socket.listenOnSocket(SERVER_BACKLOG);
    
    std::cout << " -------- Socket setup complete -------- " << std::endl;

    return 0;
}

void AuthServer::loadUserDB(const std::string &userdb) {
    std::cout << std::endl << " ----------- Loading user db file ----------- " << std::endl;
    std::ifstream is(userdb);
    if (is.fail()) {
        std::cerr << "Failed to open user db file" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (getline(is, line)) {
        std::string username, password;
        std::stringstream ss(line);
        ss >> username;
        ss >> password;
        _users[username] = password;
    }

    std::cout << "Size: " << _users.size() << std::endl;
    std::cout << " ----------- Loaded user db file ----------- " << std::endl;
}

void AuthServer::start() {
    std::cout << std::endl << " -------- Starting Auth Server --------- " << std::endl;

    // store client address during incoming connect requests
    int client_sockfd;

    while (true) {
        client_sockfd = _listen_socket.acceptOnSocket();

        // add the connection received to the queue
        _client_requests_queue->enqueue(client_sockfd);

        std::cout << "Added client connection to queue: " << client_sockfd << std::endl;
    }
}

void AuthServer::stop() {
    std::cout << " -------- Stopping Auth Server --------- " << std::endl;

    // wait for the worker threads to complete
    _client_thread_pool.joinWorkerThreads();

    // close the TCP socket
    _listen_socket.closeSocket();
}

int main(int argc, char *argv[]) {
    // server command line arguments
    if (argc != 2) {
        std::cerr << "Incorrect number of arguments. Usage: <executable> <config_path>" << std::endl;
        exit(1);
    }

    // create an instance of Authorization server
    AuthServer auth_server(argv[1]);

    // start the Auth server and listen indefinitely for incoming client connections
    auth_server.start();

    // shutdown the Auth server
    auth_server.stop();

    return 0;
}