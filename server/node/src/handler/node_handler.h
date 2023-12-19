#pragma once

#include "../formats/message.h"
#include "../vo/request.h"
#include "../vo/response.h"

class NodeServiceHandler {
    private:
        const static int MAX_BUFFER_LENGTH = 4096;

        NodeServiceHandler();

        // Private copy constructor and copy assignment operator to prevent cloning
        NodeServiceHandler(const NodeServiceHandler&) = delete;
        NodeServiceHandler& operator=(const NodeServiceHandler&) = delete;
    public:
        static NodeServiceHandler* getInstance();

        int writeNodeConnectRequest(int, NodeConnectRequest*);
        LeaderResponse* readLeaderResponse(int);

        Request * readFileRequest(int);
        int writeFileReceiptResponse(int, Response *);
};