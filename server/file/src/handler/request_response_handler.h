#pragma once

#include "../vo/request.h"
#include "../vo/response.h"
#include "../config/configs.h"

class RequestResponseHandler {
    private:
        const static int MAX_BUFFER_LENGTH = 4096;

        RequestResponseHandler();

        // Private copy constructor and copy assignment operator to prevent cloning
        RequestResponseHandler(const RequestResponseHandler&) = delete;
        RequestResponseHandler& operator=(const RequestResponseHandler&) = delete;

    public:
        static RequestResponseHandler* getInstance();

        Request* readRequest(int);
        int writeResponse(int, Response *);
};