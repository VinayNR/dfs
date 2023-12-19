#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

class NetSocket {
    private:
        int sockfd_; // the underlying socket file descriptor

        static int waitOnSocket(int, int);

    public:
        NetSocket();

        int getSocketID() const;

        // network socket operations
        void createSocket();
        void closeSocket();

        int connectSocket(const struct addrinfo *);
        int bindSocket(const char *);
        int listenOnSocket(int);
        int acceptOnSocket();
        
        static int send(int, const char *, size_t);
        static int receive(int, char *&, int, int);
        
};