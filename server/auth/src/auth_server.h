#pragma once

#include "net/sockets.h"
#include "concurrency/threadpool.h"
#include "concurrency/queue.h"
#include "handler/auth_handler.h"
#include "config/configs.h"

#include <string>
#include <unordered_map>

class AuthServer {
    private:
        // network socket for listening for client connections
        NetSocket _listen_socket;

        // thread pool for concurrency
        ThreadPool _client_thread_pool;

        // request handler
        AuthServiceHandler *_auth_handler;

        // client request queue
        Queue<int> *_client_requests_queue;

        // map of username and passwords for users
        std::unordered_map<std::string, std::string> _users;

        // authorization server configs
        AuthConfigs _configs;

        // queue size for incoming connect requests on this Auth server
        const static int SERVER_BACKLOG = 100;

        // private member function for handling individual client requests
        void processClientRequests();

    public:
        AuthServer(const char *);

        /*
        * Basic server operations
        * Setup a listening socket, start the server and stop the server
        */
        int setupSocket(int);
        void start();
        void stop();

        void loadUserDB(const std::string &);
        std::unordered_map<std::string, std::string> getUsers();
};