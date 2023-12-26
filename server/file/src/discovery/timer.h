#pragma once

#include <memory>
#include <atomic>
#include <functional>
#include <iostream>
#include <chrono>

#include "../vo/node.h"

using NodeTimerCallback = std::function<void(INode)>;

class NodeTimer {
    private:
        int _timeout;
        INode _node;
        NodeTimerCallback _callback;

        std::atomic<bool> _is_expired;
        std::atomic<bool> _timer_reset;

        NodeTimer(int, INode, NodeTimerCallback);

    public:
        void setTimerExpiry(bool);
        bool getTimerExpiry();

        void reset();
        void unset();
        bool isTimerReset();

        static NodeTimer* createTimer(int, INode, NodeTimerCallback);

        void run();
};

NodeTimer::NodeTimer(int timeout, INode node, NodeTimerCallback cb = nullptr) {
    _timeout = timeout;
    _node = node;
    _callback = cb;
    _is_expired = false;
}

NodeTimer* NodeTimer::createTimer(int timeout, INode node, NodeTimerCallback cb) {
    return new NodeTimer(timeout, node, cb);
}

// getters and setters
void NodeTimer::setTimerExpiry(bool expired) {
    _is_expired.store(expired, std::memory_order_relaxed);
}

bool NodeTimer::getTimerExpiry() {
    return _is_expired.load(std::memory_order_relaxed);
}

void NodeTimer::reset() {
    _timer_reset.store(true, std::memory_order_relaxed);
}

void NodeTimer::unset() {
    _timer_reset.store(false, std::memory_order_relaxed);
}

bool NodeTimer::isTimerReset() {
    return _timer_reset.load(std::memory_order_relaxed);
}

void NodeTimer::run() {
    std::cout << "Starting timer with " << 2*_timeout << " seconds" << std::endl;
    while (true) {
        // sleep for timeout seconds and wake up and notify
        std::this_thread::sleep_for(std::chrono::seconds(2*_timeout));

        // check if timer was reset
        if (isTimerReset()) {
            // unset the timer, and start over
            unset();
            continue;
        }

        // the timer was not reset to run again

        // set that the timer is expired and must be notified
        setTimerExpiry(true);

        // use the callback to notify
        _callback(_node);
        break;
    }
    
}