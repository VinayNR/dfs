#pragma once

#include <iostream>

struct TreeNode {
    int key;
    TreeNode *left;
    TreeNode *right;

    TreeNode(int k, TreeNode *l, TreeNode *r) : key(k), left(l), right(r) {}
};

class BSTree {
    private:
        TreeNode *_root;

        TreeNode* findMinNode(TreeNode *);
        void inorderTraversal(TreeNode *);
        TreeNode* insert(TreeNode *, TreeNode *);
        TreeNode* remove(TreeNode *, int);

    public:
        BSTree();
        void insert(TreeNode *);
        void remove(TreeNode *);
        void remove(int);

        void printInOrder();
        int getInOrderSuccessor(int);
};

BSTree::BSTree() {
    _root = nullptr;
}

void BSTree::insert(TreeNode *node) {
    _root = insert(_root, node);
}

void BSTree::remove(TreeNode *node) {
    _root = remove(_root, node->key);
}

void BSTree::remove(int key) {
    remove(_root, key);
}

void BSTree::inorderTraversal(TreeNode *root) {
    if (root != nullptr) {
        inorderTraversal(root->left);
        std::cout << "Node: " << root->key << std::endl;
        inorderTraversal(root->right);
    }
}

int BSTree::getInOrderSuccessor(int key) {
    TreeNode *temp = _root;
    int successor = -1;
    while (temp != nullptr) {
        if (temp->key == key) return key;

        if (temp->key > key) {
            successor = temp->key;
            temp = temp->left;
        }
        else {
            temp = temp->right;
        }
    }

    // if not found, return the smallest in the tree (making it cyclic)
    if (successor == -1) return findMinNode(_root)->key;
    
    return successor;
}

void BSTree::printInOrder() {
    inorderTraversal(_root);
}

TreeNode * BSTree::findMinNode(TreeNode *root) {
    if (root == nullptr) return root;
    while (root->left != nullptr) {
        root = root->left;
    }
    return root;
}

TreeNode* BSTree::insert(TreeNode* root, TreeNode *new_node) {
    if (root == nullptr) return new_node;

    if (root->key > new_node->key) {
        root->left = insert(root->left, new_node);
    }
    else {
        root->right = insert(root->right, new_node);
    }

    return root;
}

TreeNode* BSTree::remove(TreeNode *root, int key) {
    if (root == nullptr) {
        std::cerr << "Not found in the tree" << std::endl;
        return root;
    }
    if (root->key > key) {
        root->left = remove(root->left, key);
    }
    else if (root->key < key) {
        root->right = remove(root->right, key);
    }
    else { // node found to delete
        // leaf node
        if (root->left == nullptr && root->right == nullptr) {
            delete root;
            return nullptr;
        }
        else if (root->right == nullptr) { // left child only
            TreeNode *temp = root->left;
            delete root;
            return temp;
        }
        else if (root->left == nullptr) { // right child only
            TreeNode *temp = root->right;
            delete root;
            return temp;
        }
        // both children present
        TreeNode *in_order_node = findMinNode(root->right); // in order successor
        root->key = in_order_node->key;

        root->right = remove(in_order_node, in_order_node->key);
    }

    return root;
}