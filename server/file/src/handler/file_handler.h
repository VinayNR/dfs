#pragma once

#include "../vo/file_request.h"
#include "../vo/file_response.h"

class FileHandler {
    private:
        const static int MAX_BUFFER_LENGTH = 4096;

        FileHandler();

        // Private copy constructor and copy assignment operator to prevent cloning
        FileHandler(const FileHandler&) = delete;
        FileHandler& operator=(const FileHandler&) = delete;

    public:
        static FileHandler* getInstance();

        FileRequest* readRequest(int);
        int writeResponse(int, FileResponse *);
};