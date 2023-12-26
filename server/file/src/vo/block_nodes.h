#pragma once

#include "node.h"

struct BlockNodeLocation {
    INode block_node;

    bool is_primary;
    bool is_synced;
};