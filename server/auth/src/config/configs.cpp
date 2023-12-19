#include <fstream>
#include <iostream>

#include "configs.h"
#include "../../../../lib/json.hpp"

using json = nlohmann::json;

std::string AuthConfigs::getName() {
    return _name;
}

int AuthConfigs::getPort() {
    return _port;
}

std::string AuthConfigs::getUserDB() {
    return _userdb;
}

std::string AuthConfigs::getSecret() {
    return _secret;
}

AuthConfigs AuthConfigs::loadConfigs(const std::string & filename) {
    std::cout << std::endl << " ----------- Loading server config file ----------- " << std::endl;
    AuthConfigs configs;

    try {
        std::ifstream file(filename);
        json jsonData;

        if (file.is_open()) {
            jsonData = json::parse(file);

            configs._name = jsonData["config"]["name"].get<std::string>();
            configs._port = jsonData["config"]["port"].get<int>();
            configs._userdb = jsonData["config"]["userdb"].get<std::string>();
            configs._secret = jsonData["config"]["secret"].get<std::string>();
        } else {
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