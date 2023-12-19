#pragma once

#include "../net/sockets.h"
#include "../net/utils.h"
#include "../concurrency/threadpool.h"
#include "../concurrency/queue.h"
#include "../utils/utils.h"
#include "connect.h"
#include "keyring.h"
#include "node.h"
#include "node_health.h"

#include <iostream>
#include <unordered_map>

class NodeDiscoveryListener {
    private:
        // network socket for listening for client connections
        int _listen_socket_fd;

        // network socket for UDP Heart Beats from followers
        int _keep_alive_socket_fd;

        // key ring
        KeyRing _keyring;

        // health manager for managing keep alive heart beats of followers
        NodeHealthManager _node_health_manager;

    public:
        NodeDiscoveryListener();

        void start(int, int);

        void listenForHeartBeats();

        void evictNode(INode);

        KeyRing getKeyRing();
};

// default constructor
NodeDiscoveryListener::NodeDiscoveryListener() :
    _node_health_manager([this](INode node) {
        this->evictNode(node);
    }) {
}

KeyRing NodeDiscoveryListener::getKeyRing() {
    return _keyring;
}

void NodeDiscoveryListener::listenForHeartBeats() {
    struct sockaddr *remote_address = nullptr;
    std::cout << "Listening for heart beats" << std::endl;
    NodeConnectRequest *heart_beat;
    while (true) {
        heart_beat = receiveHeartBeat(_keep_alive_socket_fd, remote_address);
        std::cout << "Heart beat received from: " << heart_beat->node.getFullDomain() << std::endl;

        _node_health_manager.recordHeartBeat(heart_beat->node.getFullDomain());

        delete remote_address;
        remote_address = nullptr;
    }
}

void NodeDiscoveryListener::start(int discovery_port, int keep_alive_port) {
    std::cout << std::endl << " -------- Starting Node Discovery Listener --------- " << std::endl;

    // setup the listening socket for the discovery server
    _listen_socket_fd = setupListeningSocket(discovery_port, SOCK_STREAM);

    _keep_alive_socket_fd = setupListeningSocket(keep_alive_port, SOCK_DGRAM);
    // std::cout << "Keep alive Socket FD: " << _keep_alive_socket_fd << " bound to " << keep_alive_port << std::endl;

    // setup a new thread to listen to heart beats
    std::thread heart_beat_thread(&NodeDiscoveryListener::listenForHeartBeats, this);

    // store client address during incoming connect requests
    int client_sockfd;

    while (true) {
        client_sockfd = SocketOps::acceptOnSocket(_listen_socket_fd);

        std::cout << "Received a request on socket: " << client_sockfd << std::endl;

        // read the request
        NodeConnectRequest *request = readNodeConnectRequest(client_sockfd);

        if (request == nullptr) {
            SocketOps::closeSocket(client_sockfd);
            continue;
        }

        // process the request
        if (request->request_type == "CONNECT") {
            // hash the endpoint of the node connection request
            unsigned int node_digest = computeMD5(request->node.getFullDomain());

            // set the MD5 hash of the node
            request->node.node_digest = node_digest;

            _keyring.add(request->node);

            std::cout << "Added " << request->node.getFullDomain() << " as part of the cluster" << std::endl;

            // start a timer for the node with the keep alive timeout value
            _node_health_manager.createNodeTimer(request->heartbeat_interval, request->node);

            // send a response back
            LeaderResponse *response = new LeaderResponse{"200", "OK"};
            writeLeaderResponse(client_sockfd, response);
        }

        SocketOps::closeSocket(client_sockfd);
    }

    heart_beat_thread.join();
}

void NodeDiscoveryListener::evictNode(INode node) {
    // evict the node from the cluster
    _keyring.remove(node.node_digest);

    std::cout << "Removed " << node.getFullDomain() << " from the cluster" << std::endl;
}