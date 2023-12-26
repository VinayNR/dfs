#pragma once

#include "../vo/file_request.h"
#include "../vo/file_response.h"
#include "../vo/node_request.h"
#include "../vo/leader_response.h"

class NodeServiceHandler {
    private:
        const static int MAX_BUFFER_LENGTH = 4096;

        NodeServiceHandler();

        // Private copy constructor and copy assignment operator to prevent cloning
        NodeServiceHandler(const NodeServiceHandler&) = delete;
        NodeServiceHandler& operator=(const NodeServiceHandler&) = delete;
    public:
        static NodeServiceHandler* getInstance();

        int writeNodeRequest(int, NodeRequest*);
        LeaderResponse* readLeaderResponse(int);

        FileRequest * readFileRequest(int);
        int writeFileReceiptResponse(int, FileResponse *);
};