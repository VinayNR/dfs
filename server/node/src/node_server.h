#pragma once

#include "net/sockets.h"
#include "cache/cache.h"
#include "concurrency/threadpool.h"
#include "concurrency/queue.h"
#include "config/configs.h"
#include "handler/node_handler.h"
#include "handler/file_handler.h"

#include "vo/node.h"

class NodeServer {
    private:
        // socket for listening to client file requests
        int _listen_socket_fd;

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

        // handler for reading and writing files to disk
        FileHandler *_file_handler;

        // cache of the block to their status
        Cache<std::string, bool> _block_cache;

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