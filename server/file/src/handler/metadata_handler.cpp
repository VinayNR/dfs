#include "metadata_handler.h"

#include "../net/sockets.h"
#include "../utils/utils.h"

#include <cmath>

// default constructor
MetadataHandler::MetadataHandler() {}

MetadataHandler* MetadataHandler::getInstance() {
    static MetadataHandler instance;
    return &instance;
}

std::vector<Block> MetadataHandler::calculateBlocks(std::string unique_file_uuid, size_t filesize, size_t min_chunk_size_bytes, u_int16_t max_chunks) {
    // decide the number of chunks
    size_t chunk_size = min_chunk_size_bytes;
    u_int16_t number_of_chunks = static_cast<u_int16_t>(std::ceil(static_cast<double>(filesize) / chunk_size));
    if (number_of_chunks > max_chunks) {
        number_of_chunks = max_chunks;
        chunk_size = static_cast<size_t>(std::ceil(static_cast<double>(filesize) / number_of_chunks));
    }
    
    // determine block locations
    std::vector<size_t> block_boundaries = determineBlockBoundaries(filesize, number_of_chunks, chunk_size);
    std::vector<Block> blocks;

    size_t start = 0, end = -1;
    for (uint32_t block_num = 0; block_num < block_boundaries.size(); block_num++) {
        end = block_boundaries.at(block_num);
        blocks.push_back(Block{block_num, unique_file_uuid, start, end-1});
        start = end;
    }

    // add the last block
    blocks.push_back(Block{block_boundaries.size(), unique_file_uuid, start, filesize-1, {}});

    return blocks;
}

std::vector<size_t> MetadataHandler::determineBlockBoundaries(size_t size, u_int16_t number_of_chunks, size_t chunk_size) {
    std::vector<size_t> block_boundaries;
    for (int i=1; i<number_of_chunks; i++) {
        block_boundaries.push_back(i*chunk_size);
        std::cout << "Block boundary: " << block_boundaries.at(i-1) << std::endl;
    }

    return block_boundaries;
}