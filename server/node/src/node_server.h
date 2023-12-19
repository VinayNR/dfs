#pragma once

#include "net/sockets.h"
#include "concurrency/threadpool.h"
#include "concurrency/queue.h"
#include "config/configs.h"
#include "handler/node_handler.h"

class NodeServer {
    private:
        // socket for listening to client file requests
        int _listen_socket_fd;

        // socket for discovery
        int _discovery_socket_fd;

        // UDP socket for sending heart beats
        int _heart_beats_socket_fd;

        // configs
        NodeServerConfigs _configs;

        // thread pool for concurrency
        ThreadPool _client_thread_pool;

        // client request queue
        Queue<int> *_client_requests_queue;

        // handler for client requests
        NodeServiceHandler *_node_handler;

        void processClientRequests();

    public:
        NodeServer(const char *);

        void setupSocket();
        void createClientThreadPool(int);

        /*
        * Basic server operations
        * Setup a listening socket, start the server and stop the server
        */
        void start();
        void stop();

        // cluster operations
        int discover();
        void sendKeepaliveHeartBeats();

        struct sockaddr * constructDiscoveryServerAddress();
};