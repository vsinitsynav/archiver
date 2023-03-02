#include "constants.h"

std::string InvalidCommand::Errortype() {
    return "Command is invalid. Please, read the help message.";
}

std::string EmptyContainer::Errortype() {
    return "The container is empty";
}