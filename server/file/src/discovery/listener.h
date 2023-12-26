#pragma once

#include "../net/sockets.h"
#include "../net/utils.h"
#include "../concurrency/threadpool.h"
#include "../concurrency/queue.h"
#include "../vo/node_request.h"
#include "../handler/node_handler.h"
#include "../utils/utils.h"
#include "keyring.h"
#include "node_health.h"

#include <iostream>
#include <unordered_map>

using NodeBlockCacheCallback = std::function<void(std::string, std::vector<std::string>)>;

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

        // node handler object
        NodeHandler *_node_handler;

        // callback for updating the metadata cache server
        NodeBlockCacheCallback _cb;

    public:
        NodeDiscoveryListener(NodeBlockCacheCallback);

        void start(int, int);

        void listenForHeartBeats();

        void evictNode(INode);

        KeyRing getKeyRing();

        NodeRequest* receiveHeartBeat(int, struct sockaddr *&);
};

// default constructor
NodeDiscoveryListener::NodeDiscoveryListener(NodeBlockCacheCallback cb) :
    _node_health_manager([this](INode node) {
        this->evictNode(node);
    }) {
        // get an instance of the file metadata handler
        _node_handler = NodeHandler::getInstance();

        _cb = cb;
}

KeyRing NodeDiscoveryListener::getKeyRing() {
    return _keyring;
}


NodeRequest* NodeDiscoveryListener::receiveHeartBeat(int sockfd, struct sockaddr *& remote_address) {
    socklen_t serverlen = sizeof(struct sockaddr);

    // create the packet buffer data that is filled by the network
    char *buffer = new char[4096];
    memset(buffer, 0, 4096);
    
    // Receive a packet
    int bytesReceived = recvfrom(sockfd, buffer, 4096, 0, remote_address, &serverlen);

    if (bytesReceived == -1)  {
        return nullptr;
    }

    // deserialize the buffer
    NodeRequest *request = NodeRequest::deserialize(buffer);

    // clean up pointers
    deleteAndNullifyPointer(buffer, true);

    return request;
}

void NodeDiscoveryListener::listenForHeartBeats() {
    struct sockaddr *remote_address = nullptr;
    std::cout << "Listening for heart beats" << std::endl;
    NodeRequest *heart_beat;
    while (true) {
        heart_beat = receiveHeartBeat(_keep_alive_socket_fd, remote_address);
        std::cout << "Heart beat received from: " << heart_beat->node.getFullDomain() << std::endl;

        // record the heart beat
        _node_health_manager.recordHeartBeat(heart_beat->node.getFullDomain());

        // update the metadata cache object
        _cb(heart_beat->node.getFullDomain(), heart_beat->blocks);

        delete remote_address;
        remote_address = nullptr;
    }
}

void NodeDiscoveryListener::start(int discovery_port, int keep_alive_port) {
    std::cout << std::endl << " -------- Starting Node Discovery Listener --------- " << std::endl;

    // setup the listening socket for the discovery server
    _listen_socket_fd = setupListeningSocket(discovery_port, SOCK_STREAM);

    // setup the listening socket for the keep alive server
    _keep_alive_socket_fd = setupListeningSocket(keep_alive_port, SOCK_DGRAM);

    // setup a new thread to listen to heart beats
    std::thread heart_beat_thread(&NodeDiscoveryListener::listenForHeartBeats, this);

    // store client address during incoming connect requests
    int client_sockfd;

    while (true) {
        client_sockfd = SocketOps::acceptOnSocket(_listen_socket_fd);

        std::cout << "Received a request on socket: " << client_sockfd << std::endl;

        // read the request
        NodeRequest *request = _node_handler->readRequest(client_sockfd);

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
            _node_handler->writeResponse(client_sockfd, response);
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