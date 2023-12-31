#pragma once

#include "../formats/auth_format.h"

class AuthServiceHandler {
    private:
        const static int MAX_BUFFER_LENGTH = 4096;

        AuthServiceHandler();

        // Private copy constructor and copy assignment operator to prevent cloning
        AuthServiceHandler(const AuthServiceHandler&) = delete;
        AuthServiceHandler& operator=(const AuthServiceHandler&) = delete;

    public:
        static AuthServiceHandler* getInstance();

        AuthRequest* readRequest(int);
        int writeResponse(int, AuthResponse*);
};