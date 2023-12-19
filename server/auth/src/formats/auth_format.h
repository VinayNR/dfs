#pragma once

#include <string>

struct AuthRequest {
    std::string request_type;
    std::string username;
    std::string password;
};

struct AuthResponse {
    std::string response_code;
    std::string response_message;
    std::string access_token;
};

AuthRequest* deserializeRequest(const char *);
int serializeResponse(const AuthResponse *, char *&);