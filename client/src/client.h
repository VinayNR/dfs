#pragma once

#include "net/sockets.h"
#include "config/client_config.h"
#include "handler/auth_handler.h"
#include "handler/client_handler.h"
#include "handler/command_handler.h"
#include "vo/request.h"
#include "vo/response.h"
#include "vo/blocks.h"

class DFSClient {
    private:
        // client config file
        ClientConfigs _configs;

        // authentication handler
        AuthServiceHandler *_auth_handler;

        // command handler
        CommandHandler *_command_handler;

        // file service handler
        ClientHandler *_client_handler;

        std::string _access_token;

        int authenticate();

    public:
        DFSClient(char *);

        void start();
        void writeBlock(const char *, Block);
        void writeBlocks(const char *, const std::vector<Block> &);
        
};