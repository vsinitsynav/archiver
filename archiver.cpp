#include <iostream>

#include "constants.h"
#include "parse_commands.h"

int main(int argc, char** argv) {
    std::string message =
        "How to use this super-puper archiver\n -h\tPrints this message \n  \
                            -c archive_name file1 [file2...]\tCompresses files file1, ..., filen to \
                            archive archive_name\n -d archive_name\tDecompresses files \
                            from archive_name and put them into current directory\n";

    Parser parser(message);
    parser.AddArgument("-c", "compress");
    parser.AddArgument("-d", "decompress");
    parser.Parse(argc, argv);
    return 0;
}
