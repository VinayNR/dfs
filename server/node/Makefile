CC = g++  # Compiler
CFLAGS = -std=c++14  # Compiler flags

nodeserver = nodeserver

config = src/config/*.cpp
formats = src/formats/*.cpp
handler = src/handler/*.cpp
net = src/net/*.cpp
utils = src/utils/*.cpp

all:
	$(CC) $(CFLAGS) src/node_server.cpp $(config) $(formats) $(handler) $(net) $(utils) -o $(nodeserver)

clean:
	rm -f $(nodeserver)

.PHONY: all clean