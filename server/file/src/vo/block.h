#pragma once

#include <string>
#include <vector>

#include "node.h"
#include "block_nodes.h"

struct Block {
    size_t block_id;
    std::string file_uuid;
    size_t start;
    size_t end;

    std::vector<BlockNodeLocation> block_locations;

    std::string getBlockAsString() {
        std::string serialized_str;
        serialized_str += (std::to_string(block_id) + " " + file_uuid + " " + std::to_string(start) + " " + std::to_string(end) + " ");

        // add the primary node details that will be holding this block
        for (const BlockNodeLocation &block_location: block_locations) {
            if (block_location.is_primary) {
                serialized_str += (block_location.block_node.ip + " " + block_location.block_node.port);
                break;
            }
        }

        return serialized_str;
    }
};