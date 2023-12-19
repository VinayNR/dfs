CC = g++  # Compiler
CFLAGS = -std=c++14  # Compiler flags

client = client

config = src/config/*.cpp
handler = src/handler/*.cpp
net = src/net/*.cpp
utils = src/utils/*.cpp

all:
	$(CC) $(CFLAGS) src/client.cpp $(config) $(handler) $(net) $(utils) -o $(client)

clean:
	rm -f $(client)

.PHONY: all clean