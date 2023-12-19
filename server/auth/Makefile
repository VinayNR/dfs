CC = g++  # Compiler
CFLAGS = -std=c++11  # Compiler flags

authserver = authserver

config = src/config/*.cpp
formats = src/formats/*.cpp
handler = src/handler/*.cpp
net = src/net/*.cpp
utils = src/utils/*.cpp

all:
	$(CC) $(CFLAGS) -I /opt/homebrew/opt/openssl@3/include -L /opt/homebrew/opt/openssl@3/lib -lcrypto src/auth_server.cpp $(config) $(formats) $(handler) $(net) $(utils) -o $(authserver)

clean:
	rm -f $(authserver)

.PHONY: all clean