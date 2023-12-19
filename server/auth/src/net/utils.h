#include <iostream>
#include <cstring>  // for memset
#include <netdb.h>  // for getaddrinfo
#include <netinet/in.h>  // for AF_INET
#include <sys/socket.h>  // for SOCK_STREAM

int getServerInfo(char *, const char *, struct addrinfo *&);
void getHostAndPort(const char *, char *&, char *&);