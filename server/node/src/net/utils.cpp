#include "utils.h"
#include "sockets.h"

int getServerInfo(const char *domain, const char *port, struct addrinfo *& serverinfo) {
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
        port = new char[3];
        memset(port, 0, 3);
        memcpy(port, "80", 2);
    }
}

int setupListeningSocket(int port, int socket_type) {
    std::cout << std::endl << "---------- Setting up listening socket ---------- " << std::endl;

    // create a tcp socket
    int listen_socket_fd = SocketOps::createSocket(socket_type);

    if (listen_socket_fd == -1) {
        std::cerr << "Failed to create a socket" << std::endl;
        return -1;
    }

    // display the socket
    std::cout << "Socket FD: " << listen_socket_fd << " bound to port: " << port << std::endl;

    /*
    * setsockopt: Handy debugging trick that lets us rerun the server immediately after we kill it; 
    * otherwise we have to wait about 20 secs. 
    */
    int optval = 1;
    setsockopt(listen_socket_fd, SOL_SOCKET, optval, static_cast<const void*>(&optval) , sizeof(int));

    // bind the socket to the port
    if (SocketOps::bindSocket(listen_socket_fd, std::to_string(port).c_str()) == -1) {
        std::cerr << "Failed to bind socket" << std::endl;
        SocketOps::closeSocket(listen_socket_fd);
        return -1;
    }

    // listen on the socket for incoming connections
    if (socket_type == SOCK_STREAM && SocketOps::listenOnSocket(listen_socket_fd, 20) == -1) {
        std::cerr << "Failed to listen on socket" << std::endl;
        SocketOps::closeSocket(listen_socket_fd);
        return -1;
    }

    std::cout << " -------- Listening socket setup complete -------- " << std::endl;

    return listen_socket_fd;
}

int connectToServer(const char *host, const char *port) {
    std::cout << " ----------- Connecting to Server ----------- " << std::endl;

    // create a new TCP socket to open a connection to the server
    int client_socket_fd = SocketOps::createSocket(SOCK_STREAM);
    if (client_socket_fd == -1) {
        std::cerr << "Failed to create a socket" << std::endl;
        return -1;
    }

    struct addrinfo *server_info = nullptr;
    getServerInfo(host, port, server_info);

    // check if the DNS lookup failed
    if (server_info == nullptr) {
        std::cerr << "Failed to obtain IP of the server" << std::endl;
        close(client_socket_fd);
        return -1;
    }

    // connect to the remote server
    if (SocketOps::connectSocket(client_socket_fd, server_info) == -1) {
        std::cerr << "Failed to connect to server" << std::endl;
        close(client_socket_fd);
        return -1;
    }

    return client_socket_fd;
}