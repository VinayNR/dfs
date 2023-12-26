#pragma once

#include "../vo/node_request.h"
#include "../vo/leader_response.h"

class NodeHandler {
    private:
        const static int MAX_BUFFER_LENGTH = 4096;

        NodeHandler();

        // Private copy constructor and copy assignment operator to prevent cloning
        NodeHandler(const NodeHandler&) = delete;
        NodeHandler& operator=(const NodeHandler&) = delete;

    public:
        static NodeHandler* getInstance();

        NodeRequest* readRequest(int);
        int writeResponse(int, LeaderResponse *);
};