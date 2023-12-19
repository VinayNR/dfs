#pragma once

#include <string>

class AuthConfigs {
    private:
        std::string _name;
        int _port;
        std::string _userdb;
        std::string _secret;

    public:
        // getters
        std::string getName();
        int getPort();
        std::string getUserDB();
        std::string getSecret();

        // loader
        static AuthConfigs loadConfigs(const std::string &);
};