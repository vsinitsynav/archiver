#pragma once
#include "constants.h"

class Parser {
public:
    explicit Parser(std::string help_message);

    void AddArgument(const std::string& short_name, const std::string& full_name);

    void Compress(int files_cnt, char** files, char* archive_name);

    void Decompress(char* dir, char* archive_name);

    void Parse(int argc, char** argv);

    void HelpMessage();

private:
    struct ArgumentsHolder {
        std::string shortName;
        std::string fullName;
    };

    std::string help_message_;
    std::vector<ArgumentsHolder> args_;
};
