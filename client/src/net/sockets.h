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

class SocketOps {
    private:
        static int waitOnSocket(int, int);

        SocketOps();

    public:
        // network socket operations
        static int createSocket(int);
        static void closeSocket(int);

        // listening socket operations
        static int connectSocket(int, const struct addrinfo *);
        static int bindSocket(int, const char *);
        static int listenOnSocket(int, int);
        static int acceptOnSocket(int);
        
        static int send(int, const char *, size_t);
        static int receive(int, char *&, int, int);

        static int sendTo(int, const char *, size_t, struct sockaddr *);
        static int receiveFrom(int, char *&, size_t, struct sockaddr *&);
        
};