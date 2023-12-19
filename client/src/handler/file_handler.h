#pragma once

class FileHandler {
    private:
        const static int MAX_BUFFER_LENGTH = 4096;

        FileHandler();

        // Private copy constructor and copy assignment operator to prevent cloning
        FileHandler(const FileHandler&) = delete;
        FileHandler& operator=(const FileHandler&) = delete;

    public:
        static FileHandler* getInstance();

        int getFile(const char *, char *&);
        int putFile(const char *, char *, int);
};