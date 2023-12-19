#pragma once

#include <string>

class NodeServerConfigs {
    private:
        std::string _name;
        int _port;
        std::string _storage_path;
        std::string _discovery_server;
        int _discovery_port;
        int _keep_alive_port;
        int _heart_beat_interval;

    public:
        // getters
        std::string getName();
        int getPort();
        std::string getStoragePath();
        std::string getDiscoveryServer();
        int getDiscoveryPort();
        int getKeepAlivePort();
        int getHeartBeatInterval();

        // loader
        static NodeServerConfigs loadConfigs(const std::string &);
};