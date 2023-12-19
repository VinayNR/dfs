CC = g++  # Compiler
CFLAGS = -std=c++14  # Compiler flags

fileserver = fileserver

config = src/config/*.cpp
discovery = src/discovery/*.cpp
handler = src/handler/*.cpp
net = src/net/*.cpp
utils = src/utils/*.cpp

all:
	$(CC) $(CFLAGS) -I /opt/homebrew/opt/openssl@3/include -L /opt/homebrew/opt/openssl@3/lib -lcrypto src/file_server.cpp $(config) $(discovery) $(handler) $(net) $(utils) -o $(fileserver)

clean:
	rm -f $(fileserver)

.PHONY: all clean