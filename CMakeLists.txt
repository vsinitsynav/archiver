add_executable(
        archiver
        parse_commands.cpp
        bit_reader.cpp
        bit_writer.cpp
        constants.cpp
        huffman.cpp
        archiver.cpp
)

add_catch(
        test_huffman
        test.cpp
        constants.cpp
        huffman.cpp
)