#pragma once

#include <string>
#include <unordered_map>

class FileServerConfigs {
    private:
        std::string _name;
        int _port;
        int _discovery_port;
        int _keep_alive_port;
        std::string _secret;
        int _min_chunk_size_bytes;
        int _max_chunks;
        int _replication_factor;

    public:
        std::string getName();
        int getPort();
        int getDiscoveryPort();
        int getKeepAlivePort();
        std::string getSecret();
        int getMinChunkSizeBytes();
        int getMaxChunks();
        int getReplicationFactor();

        // loader
        static FileServerConfigs loadConfigs(const std::string &);
};