#include <catch.hpp>

#include "constants.h"
#include "huffman.h"

TEST_CASE("Canonical Huffman code") {
    std::unordered_map<BitSymbol, uint64_t> char_frequency1;

    char_frequency1[{false, false, true}] = 15;
    char_frequency1[{false, true, false}] = 35;
    char_frequency1[{false, true, true}] = 1;
    char_frequency1[{true, false, false}] = 1;

    Huffman huffman1(char_frequency1);

    std::vector<bool> v1 = {true, false};
    std::vector<bool> v2 = {false};
    std::vector<bool> v3 = {true, true, false};
    std::vector<bool> v4 = {true, true, true};

    REQUIRE(huffman1.symbols_code[{false, false, true}] == v1);
    REQUIRE(huffman1.symbols_code[{false, true, false}] == v2);
    REQUIRE(huffman1.symbols_code[{false, true, true}] == v3);
    REQUIRE(huffman1.symbols_code[{true, false, false}] == v4);

    std::unordered_map<BitSymbol, uint64_t> char_frequency2;

    char_frequency2[{false, false, true}] = 15;
    char_frequency2[{false, true, false}] = 5;
    char_frequency2[{false, true, true}] = 6;
    char_frequency2[{true, false, false}] = 6;
    char_frequency2[{true, true, false}] = 7;

    Huffman huffman2(char_frequency2);

    std::vector<bool> w1 = {false};
    std::vector<bool> w2 = {true, false, false};
    std::vector<bool> w3 = {true, false, true};
    std::vector<bool> w4 = {true, true, false};
    std::vector<bool> w5 = {true, true, true};

    REQUIRE(huffman2.symbols_code[{false, false, true}] == w1);
    REQUIRE(huffman2.symbols_code[{false, true, false}] == w2);
    REQUIRE(huffman2.symbols_code[{false, true, true}] == w3);
    REQUIRE(huffman2.symbols_code[{true, false, false}] == w4);
    REQUIRE(huffman2.symbols_code[{true, true, false}] == w5);
}
