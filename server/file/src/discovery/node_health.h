#pragma once

#include <chrono>
#include <thread>
#include <memory>
#include <vector>
#include <iostream>
#include <functional>

#include "timer.h"
#include "node.h"

using NodeEvictionCallback = std::function<void(INode)>;

class NodeHealthManager {
    private:
        // std::unordered_map<std::string, std::unique_ptr<NodeTimer>> _node_timers;
        std::unordered_map<std::string, NodeTimer*> _node_timers;

        NodeEvictionCallback _cb;

    public:
        NodeHealthManager(NodeEvictionCallback cb): _cb(cb) {}

        void createNodeTimer(int, INode);
        void onTimerExpired(INode);

        void recordHeartBeat(std::string);
};

void NodeHealthManager::createNodeTimer(int timeout, INode node) {
    // create a new timer for the node
    _node_timers[node.getFullDomain()] = NodeTimer::createTimer(timeout, node, [this](INode node) {
        this->onTimerExpired(node);
    });

    // start the timer on a different thread
    std::thread t(&NodeTimer::run, _node_timers[node.getFullDomain()]);
    t.detach();
}

void NodeHealthManager::onTimerExpired(INode node) {
    std::cout << "Node with node domain: " << node.getFullDomain() << " is expired" << std::endl;

    // remove the associated timer
    auto it = _node_timers.find(node.getFullDomain());

    if (it != _node_timers.end()) {
        // Element with the key found, erase it
        delete it->second;
        _node_timers.erase(it);

        // notify the listener about the node timer expiry
        _cb(node);
    }
    else {
        std::cout << "Key not found: " << node.getFullDomain() << std::endl;
    }
}

void NodeHealthManager::recordHeartBeat(std::string node_key) {
    // reset the node's timer if that is not expired
    if (_node_timers.count(node_key) > 0) {
        _node_timers[node_key]->reset();
    }
}