#pragma once

#include<string>

#include "../vo/command.h"


class CommandHandler {
    private:
        const static int MAX_BUFFER_LENGTH = 4096;

        CommandHandler();

        // Private copy constructor and copy assignment operator to prevent cloning
        CommandHandler(const CommandHandler&) = delete;
        CommandHandler& operator=(const CommandHandler&) = delete;

    public:
        static CommandHandler* getInstance();

        bool validateCommand(std::string);
        Command constructCommand(std::string);
};