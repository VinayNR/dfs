#include "utils.h"

int getServerInfo(char *domain, const char *port, struct addrinfo *& serverinfo) {
    // set the hints
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPV4
    hints.ai_socktype = SOCK_STREAM;

    // get server address info structures
    if (getaddrinfo(domain, port, &hints, &serverinfo) != 0) {
        std::cout << "Error with get addr info" << std::endl;
        serverinfo = nullptr;
        return -1;
    }
    return 0;
}

void getHostAndPort(const char *fullDomain, char *& host, char *& port) {
    // Find the position of the colon (:) to separate host and port
    const char *colonPos = std::strrchr(fullDomain, ':');

    if (colonPos != nullptr) {
        // Calculate the length of the host and port
        std::size_t hostLength = colonPos - fullDomain;
        std::size_t portLength = std::strlen(colonPos + 1);

        // Allocate memory for host and port
        host = new char[hostLength + 1];
        port = new char[portLength + 1];

        // Copy host and port strings
        std::strncpy(host, fullDomain, hostLength);
        host[hostLength] = '\0';

        std::strcpy(port, colonPos + 1);
    } else {
        // No port found, set port to nullptr
        host = new char[std::strlen(fullDomain) + 1];
        std::strcpy(host, fullDomain);
        port = "80";
    }
}