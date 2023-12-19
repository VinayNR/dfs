#pragma once

#include <string>
#include <sstream>

struct AuthRequest {
    std::string request_type;
    std::string username;
    std::string password;

    int serialize(char *& buffer) {
        std::string serialized_request;
        serialized_request += ("CONNECT " + username + ":" + password + "\r\n");
        size_t size = serialized_request.length();
        buffer = new char[size + 1];
        memset(buffer, 0, size + 1);
        memcpy(buffer, serialized_request.c_str(), size);

        return size;
    }

};

struct AuthResponse {
    std::string response_code;
    std::string response_message;
    std::string access_token;

    static AuthResponse* deserialize(const char *buffer) {
        AuthResponse *auth_response = new AuthResponse;

        // Convert the char* to std::string for easier manipulation
        std::string inputString(buffer);

        // Use std::istringstream to split the string
        std::istringstream iss(inputString);

        std::string line;
        getline(iss, line, '\r');
        
        // parse first line
        // Find the position of the first space
        size_t spacePos = line.find(' ');

        // Check if a space was found
        if (spacePos != std::string::npos) {
            // Extract the two parts
            auth_response->response_code = line.substr(0, spacePos);
            auth_response->response_message = line.substr(spacePos + 1);
        }

        // ignore a \n if present
        if (iss.peek() == '\n') iss.ignore();

        if (!iss.eof()) {
            // Read the entire contents of iss into access_token
            auth_response->access_token.assign(std::istreambuf_iterator<char>(iss), std::istreambuf_iterator<char>());
        }

        return auth_response;
    }
};