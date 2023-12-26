#pragma once

#include<string>

#include "../vo/file_request.h"
#include "../vo/file_response.h"
#include "file_handler.h"


class FileHandler {
    private:
        const static int MAX_BUFFER_LENGTH = 4096;

        FileHandler();

        // Private copy constructor and copy assignment operator to prevent cloning
        FileHandler(const FileHandler&) = delete;
        FileHandler& operator=(const FileHandler&) = delete;

    public:
        static FileHandler* getInstance();

        FileResponse* readResponse(int);
        int writeRequest(int, FileRequest *);

        int handlePut(const char *, char *&);

        FileRequest * constructRequest(Command, Data, const std::string &);

        int getFile(const char *, char *&);
        int putFile(const char *, char *, int);
};