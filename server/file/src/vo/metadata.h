#pragma once

#include <string>

#include "blocks.h"

struct FileMetadata {
    std::string file_name;
    std::string owner;
    std::string unique_file_uuid;

    // block details
    u_int16_t num_blocks;
    std::vector<Block> blocks;
};