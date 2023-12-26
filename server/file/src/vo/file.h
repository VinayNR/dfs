#pragma once

#include "file_attr.h"
#include "block.h"

struct File {
    FileAttr file_attr;
    std::vector<Block> blocks;
};