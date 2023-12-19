#include "command_handler.h"

#include "../utils/utils.h"

#include <sstream>
#include <algorithm>

// default constructor
CommandHandler::CommandHandler() {}

CommandHandler* CommandHandler::getInstance() {
    static CommandHandler instance;
    return &instance;
}

bool CommandHandler::validateCommand(std::string command) {
    size_t spacePos = command.find(' ');

    // no space found
    if (spacePos == std::string::npos) {
        return false;
    }

    std::string first_part = command.substr(0, spacePos);

    // Convert first_part to lowercase
    std::transform(first_part.begin(), first_part.end(), first_part.begin(), ::tolower);

    return first_part == "get" || first_part == "put" || first_part == "list";
}

Command CommandHandler::constructCommand(std::string command) {
    // Convert command to lowercase
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

    // extract the first part
    size_t spacePos = command.find(' ');

    return Command {command.substr(0, spacePos), command.substr(spacePos + 1)};
}