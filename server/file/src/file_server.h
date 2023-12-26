#pragma once

#include "net/sockets.h"
#include "cache/cache.h"
#include "concurrency/threadpool.h"
#include "concurrency/queue.h"
#include "handler/metadata_handler.h"
#include "handler/file_handler.h"
#include "config/configs.h"
#include "discovery/listener.h"
#include "vo/file.h"

class FileServer {
    private:
        // configs for the file server
        FileServerConfigs _configs;

        // network socket for listening for client connections
        int _listen_socket_fd;

        // discovery server
        NodeDiscoveryListener _discovery_server;

        // thread pool for concurrency
        ThreadPool _client_thread_pool;

        // client request queue
        Queue<int> *_client_requests_queue;

        // metadata handler
        MetadataHandler *_metadata_handler;

        // request response handler
        FileHandler *_file_handler;

        // file metadata cache
        Cache<std::string, File> _metadata_cache;

        const static int SERVER_BACKLOG = 30;

        void processClientRequests();

        void updateMetadataCache(std::string, std::vector<std::string>);

    public:
        FileServer(const char *);

        void createClientThreadPool(int);
        void setupSocket();

        /*
        * Basic server operations
        * Setup a listening socket, start the server and stop the server
        */
        void start();
        void stop();

        void startDiscovery();
};