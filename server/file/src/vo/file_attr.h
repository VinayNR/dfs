#pragma once

#include <string>

struct FileAttr {
    std::string file_name;
    std::string file_owner;
    std::string file_uuid;
    size_t file_size;
};