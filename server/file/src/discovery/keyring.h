#pragma once

#include "tree.h"
#include "../vo/node.h"

class KeyRing {
    private:
        // Binary search tree
        BSTree *_tree;

        // map of hash key to node
        std::unordered_map<u_int32_t, INode> _node_map;

        const static size_t _ring_size = 65537;

    public:
        KeyRing();
        std::unordered_map<u_int32_t, INode> getNodeMap();

        void add(INode);
        void remove(u_int32_t);

        INode getNodeForKey(u_int32_t);
        INode getNextNode(const INode &);
        INode getNextNode(u_int32_t);
};

KeyRing::KeyRing() {
    _tree = new BSTree();
}

std::unordered_map<u_int32_t, INode> KeyRing::getNodeMap() {
    return _node_map;
}

void KeyRing::add(INode node) {
    if (_node_map.count(node.node_digest%_ring_size) > 0) {
        std::cerr << "Server already part of the ring" << std::endl;
        return;
    }

    // make a new tree node
    TreeNode *new_node = new TreeNode(node.node_digest%_ring_size, nullptr, nullptr);
    _tree->insert(new_node);

    // add the hash to the map
    _node_map[node.node_digest%_ring_size] = node;
}

void KeyRing::remove(u_int32_t node_digest) {
    if (_node_map.count(node_digest%_ring_size) == 0) {
        std::cerr << "Failed to find the node to evict, probably removed earlier" << std::endl;
        return;
    }

    // remove an element from the key ring
    _tree->remove(node_digest%_ring_size);

    _node_map.erase(node_digest%_ring_size);
}

INode KeyRing::getNodeForKey(u_int32_t key) {
    // return in order successor in the key ring
    return _node_map[_tree->getInOrderSuccessor(key%_ring_size)];
}

INode KeyRing::getNextNode(u_int32_t node_pos) {
    return _node_map[_tree->getInOrderSuccessor(node_pos%_ring_size + 1)];
}

INode KeyRing::getNextNode(const INode &node) {
    return _node_map[_tree->getInOrderSuccessor(node.node_digest%_ring_size + 1)];
}