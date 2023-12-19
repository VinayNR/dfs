#include "file_handler.h"

#include <dirent.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

// default constructor
FileHandler::FileHandler() {}

FileHandler* FileHandler::getInstance() {
    static FileHandler instance;
    return &instance;
}

int FileHandler::getFile(const char * filename, char *& fileContents) {
    std::ifstream filestream (filename, std::ios::binary | std::ios::ate);
    if (!filestream.is_open()) {
        std::cerr << "Unable to open file" << std::endl;
        return -1;
    }

    int fileSize = filestream.tellg();
    filestream.seekg(0, std::ios::beg);

    fileContents = new char[fileSize + 1];
    memset(fileContents, 0, fileSize + 1);

    if (!filestream.read(fileContents, fileSize)) {
        std::cerr << "Unable to read file" << std::endl;
        return -1;
    }

    filestream.close();
    return fileSize;
}

int FileHandler::putFile(const char * filename, char * fileContents, int dataSize) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile.is_open()) {
        return -1;
    }

    // Write dataSize bytes from fileContents into the file
    outFile.write(fileContents, dataSize);

    if (outFile.fail()) {
        return -1;
    }

    outFile.close();
    return 0;
}