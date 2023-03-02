
#include "parse_commands.h"

#include "encode.cpp"
#include "decode.cpp"

Parser::Parser(std::string help_message) : help_message_(help_message) {
}

void Parser::HelpMessage() {
    std::cout << help_message_ << "\n";
}

void Parser::AddArgument(const std::string& short_name, const std::string& full_name) {
    args_.push_back({short_name, full_name});
}

void Parser::Parse(int argc, char** argv) {
    if (strcmp(*(argv + 1), "-h") == 0) {
        HelpMessage();
        return;
    }
    if (strcmp(*(argv + 1), "-c") == 0) {
        const std::string& output_file = *(argv + 2);
        std::vector<std::string> filepaths;
        for (int i = 3; i < argc; ++i) {
            filepaths.push_back(*(argv + i));
        }
        EncodeFiles(output_file, filepaths);
    }
    if (strcmp(*(argv + 1), "-d") == 0) {
        std::vector<std::string> filepaths;
        DecodeFile(*(argv + 2), filepaths);
    }
}