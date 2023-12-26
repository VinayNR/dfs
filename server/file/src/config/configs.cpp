#include "configs.h"
#include "../../../../lib/json.hpp"

#include <iostream>
#include <fstream>

using json = nlohmann::json;

std::string FileServerConfigs::getName() {
    return _name;
}

int FileServerConfigs::getPort() {
    return _port;
}

int FileServerConfigs::getDiscoveryPort() {
    return _discovery_port;
}

int FileServerConfigs::getKeepAlivePort() {
    return _keep_alive_port;
}

std::string FileServerConfigs::getSecret() {
    return _secret;
}

int FileServerConfigs::getMinChunkSizeBytes() {
    return _min_chunk_size_bytes;
}

int FileServerConfigs::getMaxChunks() {
    return _max_chunks;
}

int FileServerConfigs::getReplicationFactor() {
    return _replication_factor;
}

// loader
FileServerConfigs FileServerConfigs::loadConfigs(const std::string &filename) {
    std::cout << std::endl << " ----------- Loading server config file ----------- " << std::endl;
    FileServerConfigs configs;

    try {
        std::ifstream file(filename);
        json jsonData;

        if (file.is_open()) {
            jsonData = json::parse(file);
            configs._name = jsonData["config"]["name"].get<std::string>();
            configs._port = jsonData["config"]["port"].get<int>();
            configs._discovery_port = jsonData["config"]["discoveryPort"].get<int>();
            configs._keep_alive_port = jsonData["config"]["keepAlivePort"].get<int>();
            configs._secret = jsonData["config"]["secret"].get<std::string>();
            configs._min_chunk_size_bytes = jsonData["config"]["minChunkSizeBytes"].get<int>();
            configs._max_chunks = jsonData["config"]["maxChunks"].get<int>();
            configs._replication_factor = jsonData["config"]["replicationFactor"].get<int>();
        }
        else {
            std::cerr << "Error opening JSON file: " << filename << std::endl;
            // Handle the error as appropriate for your application
        }
    } catch (const json::exception& e) {
        std::cerr << "Error parsing JSON config: " << e.what() << std::endl;
        // Handle the error as appropriate for your application
    }

    std::cout << " ----------- Loaded server config file ----------- " << std::endl;

    return configs;
}