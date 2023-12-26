#pragma once

#include <string>
#include <vector>

#include "node.h"
#include "node.h"

struct Block {
    size_t block_id;
    std::string file_uuid;
    size_t start;
    size_t end;

    INode primary_node;
};