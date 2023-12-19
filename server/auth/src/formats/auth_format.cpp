#include "auth_format.h"

#include <sstream>
#include <iostream>

AuthRequest* deserializeRequest(const char *buffer) {
    AuthRequest* auth_request = new AuthRequest;

    // Convert the char* to std::string for easier manipulation
    std::string inputString(buffer);

    // Use std::istringstream to split the string
    std::istringstream iss(inputString);

    // Read the first word (CONNECT)
    iss >> auth_request->request_type;

    // ignore a space if it follows this
    if (iss.peek() == ' ') {
        iss.ignore();
    }

    // Check if the command is "CONNECT"
    if (auth_request->request_type == "CONNECT") {
        // Read the rest of the line (username:password)
        std::getline(iss, auth_request->username, ':');
        std::getline(iss, auth_request->password, '\r');
    }
    else return nullptr;

    return auth_request;
}

int serializeResponse(const AuthResponse *auth_response, char *& buffer) {
    std::cout << " ----------- Inside Serializer ----------- " << std::endl;

    std::string serialized_response;
    std::cout << "auth_response response code: " << auth_response->response_code << std::endl;
    serialized_response += (auth_response->response_code + " " + auth_response->response_message + "\r\n" + auth_response->access_token);

    std::cout << "Serialzied string: " << serialized_response << std::endl;
    size_t size = serialized_response.length();

    buffer = new char[size + 1];
    memset(buffer, 0, size + 1);
    memcpy(buffer, serialized_response.c_str(), size);

    std::cout << "Buffer: " << buffer << std::endl;
    std::cout << "Buffer Size: " << size << std::endl;

    std::cout << " ----------- Finished Serializer ----------- " << std::endl;
    return size;
}