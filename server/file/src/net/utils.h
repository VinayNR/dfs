#include <iostream>
#include <cstring>  // for memset
#include <netdb.h>  // for getaddrinfo
#include <netinet/in.h>  // for AF_INET
#include <sys/socket.h>  // for SOCK_STREAM
#include <unistd.h>

int getServerInfo(const char *, const char *, struct addrinfo *&);
void getHostAndPort(const char *, char *&, char *&);
int setupListeningSocket(int, int);
int connectToServer(const char *, const char *);