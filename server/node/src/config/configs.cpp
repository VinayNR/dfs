#include <fstream>
#include <iostream>

#include "configs.h"
#include "../../../../lib/json.hpp"

using json = nlohmann::json;

std::string NodeServerConfigs::getName() {
    return _name;
}

int NodeServerConfigs::getPort() {
    return _port;
}

std::string NodeServerConfigs::getStoragePath() {
    return _storage_path;
}

std::string NodeServerConfigs::getDiscoveryServer() {
    return _discovery_server;
}

int NodeServerConfigs::getDiscoveryPort() {
    return _discovery_port;
}

int NodeServerConfigs::getKeepAlivePort() {
    return _keep_alive_port;
}

int NodeServerConfigs::getHeartBeatInterval() {
    return _heart_beat_interval;
}

NodeServerConfigs NodeServerConfigs::loadConfigs(const std::string & filename) {
    std::cout << std::endl << " ----------- Loading node server config file ----------- " << std::endl;
    NodeServerConfigs configs;

    try {
        std::ifstream file(filename);
        json jsonData;

        if (file.is_open()) {
            jsonData = json::parse(file);

            configs._name = jsonData["config"]["name"].get<std::string>();
            configs._port = jsonData["config"]["port"].get<int>();
            configs._storage_path = jsonData["config"]["storagePath"].get<std::string>();
            configs._discovery_server = jsonData["config"]["discoveryServer"].get<std::string>();
            configs._discovery_port = jsonData["config"]["discoveryPort"].get<int>();
            configs._keep_alive_port = jsonData["config"]["keepAlivePort"].get<int>();
            configs._heart_beat_interval = jsonData["config"]["heartbeatInterval"].get<int>();
        } else {
            std::cerr << "Error opening JSON file: " << filename << std::endl;
            // Handle the error as appropriate for your application
        }
    } catch (const json::exception& e) {
        std::cerr << "Error parsing JSON config: " << e.what() << std::endl;
        // Handle the error as appropriate for your application
    }

    std::cout << " ----------- Loaded node server config file ----------- " << std::endl;

    return configs;
}