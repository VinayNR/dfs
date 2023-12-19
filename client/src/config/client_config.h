#pragma once

#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>


class ClientConfigs {
    private:
        std::string _username;
        std::string _password;
        std::unordered_map<std::string, std::string> _servers;

    public:
        // getters
        std::string getUserName();
        std::string getPassword();
        std::unordered_map<std::string, std::string> getServers();

        // loader
        static ClientConfigs loadConfigs(const std::string &);
};