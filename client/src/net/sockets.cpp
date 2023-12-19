#include "sockets.h"
#include "utils.h"

// default constructor
SocketOps::SocketOps() {}

// network socket operations
int SocketOps::createSocket(int socket_type) {
    return socket(AF_INET, socket_type, 0);
}

void SocketOps::closeSocket(int sockfd) {
    close(sockfd);
}

int SocketOps::connectSocket(int sockfd, const struct addrinfo *server_info) {
    // connect to the remote server
    return connect(sockfd, server_info->ai_addr, server_info->ai_addrlen);
}

int SocketOps::bindSocket(int sockfd, const char *port) {
    struct addrinfo *local_address = nullptr;
    if (getServerInfo(NULL, port, local_address) == -1) {
        std::cerr << "Failed to get address of local machine" << std::endl;
        return -1;
    }
    return bind(sockfd, local_address->ai_addr, local_address->ai_addrlen);
}

int SocketOps::listenOnSocket(int sockfd, int backlog) {
    return listen(sockfd, backlog);
}

int SocketOps::acceptOnSocket(int sockfd) {
    // store client address during incoming connect requests
    struct sockaddr_in client_addr;
    socklen_t addr_size = sizeof(client_addr);

    int new_sock_fd = accept(sockfd, (struct sockaddr *) &client_addr, &addr_size);

    std::cout << "Accepted a request from Client IP: " << inet_ntoa(client_addr.sin_addr) << std::endl;

    return new_sock_fd;
}

int SocketOps::send(int sockfd, const char *buffer, size_t buffer_length) {
    std::cout << std::endl << " ------- Inside Send ------- " << std::endl;
    std::cout << "Trying to send: " << buffer_length << " bytes" << std::endl;
    std::cout << "Buffer: " << buffer << std::endl;
    ssize_t bytes_sent, total_sent = 0;
    while (total_sent < buffer_length) {
        bytes_sent = write(sockfd, buffer + total_sent, buffer_length - total_sent);
        if (bytes_sent <= 0) {
            perror("closed");
            return -1;
        }
        total_sent += bytes_sent;
    }
    std::cout << " ------- Sent bytes: " << total_sent << " -------- " << std::endl;
    return total_sent;
}

int SocketOps::waitOnSocket(int sockfd, int timeout_sec) {
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(sockfd, &readSet);

    struct timeval timeout;
    timeout.tv_sec = timeout_sec;
    timeout.tv_usec = 0;

    return select(sockfd + 1, &readSet, nullptr, nullptr, &timeout);
}

int SocketOps::receive(int sockfd, char *& buffer, int MAX_BUFFER_LENGTH, int timeout_sec) {
    std::cout << " ------- Inside Receive ------- " << std::endl;
    int bytes_read = -1;

    buffer = new char[MAX_BUFFER_LENGTH];
    memset(buffer, 0, MAX_BUFFER_LENGTH);

    // wait on the socket to check if data is available on the socket
    int status = SocketOps::waitOnSocket(sockfd, timeout_sec);

    if (status <= 0) {
        std::cout << "Socket: " << sockfd << " had no data in the timeout interval with status: " << status << std::endl;
        return status;
    }

    // read from the socket
    bytes_read = read(sockfd, buffer, MAX_BUFFER_LENGTH);

    std::cout << "Buffer: " << buffer << std::endl;
    std::cout << "Receive done with bytes read: " << bytes_read << std::endl;
    
    return bytes_read;
}

int SocketOps::sendTo(int sockfd, const char *buffer, size_t size, struct sockaddr *remote_address) {
    socklen_t serverlen = sizeof(struct sockaddr);

    // Send the data
    int bytesSent = sendto(sockfd, buffer, size, 0, remote_address, serverlen);

    if (bytesSent == -1) {
        std::cerr << "Error sending data: " << strerror(errno) << std::endl;
        return -1;
    }

    return bytesSent;
}

int SocketOps::receiveFrom(int sockfd, char *& buffer, size_t MAX_BUFFER_LENGTH, struct sockaddr *& remote_address) {
    socklen_t serverlen = sizeof(struct sockaddr);

    buffer = new char[MAX_BUFFER_LENGTH];
    memset(buffer, 0, MAX_BUFFER_LENGTH);
    
    // Receive a packet
    int bytes_read = recvfrom(sockfd, buffer, MAX_BUFFER_LENGTH, 0, remote_address, &serverlen);

    if (bytes_read == -1)  {
        std::cerr << "Received nothing on the socket" << std::endl;
        return -1;
    }

    std::cout << "Buffer: " << buffer << std::endl;
    std::cout << "Receive done with bytes read: " << bytes_read << std::endl;

    return bytes_read;
}