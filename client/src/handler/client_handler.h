#pragma once

#include<string>

#include "../vo/request.h"
#include "../vo/response.h"
#include "file_handler.h"


class ClientHandler {
    private:
        const static int MAX_BUFFER_LENGTH = 4096;

        ClientHandler();

        // Private copy constructor and copy assignment operator to prevent cloning
        ClientHandler(const ClientHandler&) = delete;
        ClientHandler& operator=(const ClientHandler&) = delete;

        FileHandler *_file_handler;

    public:
        static ClientHandler* getInstance();

        Response* readResponse(int);
        int writeRequest(int, Request *);

        int handlePut(const char *, char *&);

        Request * constructRequest(Command, Data, const std::string &);
};