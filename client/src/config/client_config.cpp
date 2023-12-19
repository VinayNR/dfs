#include "client_config.h"
#include "../../../lib/json.hpp"

using json = nlohmann::json;

std::string ClientConfigs::getUserName() {
    return _username;
}

std::string ClientConfigs::getPassword() {
    return _password;
}

std::unordered_map<std::string, std::string> ClientConfigs::getServers() {
    return _servers;
}

ClientConfigs ClientConfigs::loadConfigs(const std::string & filename) {
    ClientConfigs configs;

    try {
        std::ifstream file(filename);
        json jsonData;

        if (file.is_open()) {
            jsonData = json::parse(file);

            configs._username = jsonData["config"]["username"].get<std::string>();
            configs._password = jsonData["config"]["password"].get<std::string>();

            for (const auto& server : jsonData["config"]["server"].items()) {
                configs._servers[server.key()] = server.value().get<std::string>();
            }
        } else {
            std::cerr << "Error opening JSON file: " << filename << std::endl;
            // Handle the error as appropriate for your application
        }
    } catch (const json::exception& e) {
        std::cerr << "Error parsing JSON config: " << e.what() << std::endl;
        // Handle the error as appropriate for your application
    }

    return configs;
}