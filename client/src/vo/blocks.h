#pragma once

#include <string>

#include "location.h"

struct Block {
    std::string block_id;
    size_t start;
    size_t end;
    ResourceLocation location;

    std::string getBlockAsString() {
        std::string serialized_str;
        serialized_str += (block_id + " " + std::to_string(start) + " " + std::to_string(end) + " " + location.ip + " " + std::to_string(location.port));
        return serialized_str;
    }
};