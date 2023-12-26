#pragma once

#include "../config/configs.h"
#include "../vo/block.h"

class MetadataHandler {
    private:
        const static int MAX_BUFFER_LENGTH = 4096;

        MetadataHandler();

        // Private copy constructor and copy assignment operator to prevent cloning
        MetadataHandler(const MetadataHandler&) = delete;
        MetadataHandler& operator=(const MetadataHandler&) = delete;

    public:
        static MetadataHandler* getInstance();

        std::vector<Block> calculateBlocks(std::string, size_t, size_t, u_int16_t);
        std::vector<size_t> determineBlockBoundaries(size_t, u_int16_t, size_t);
};