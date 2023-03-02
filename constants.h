#pragma once

#include <algorithm>
#include <cassert>
#include <climits>
#include <cstdint>
#include <fstream>
#include <istream>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <string>
#include <string.h>
#include <unordered_map>
#include <utility>
#include <vector>
#include <queue>

using BitSymbol = std::vector<bool>;
using Operation = void(const std::string&, const std::vector<std::string>&);

const BitSymbol FILENAME_END = {true, false, false, false, false, false, false, false, false};
const BitSymbol ONE_MORE_FILE = {true, false, false, false, false, false, false, false, true};
const BitSymbol ARCHIVE_END = {true, false, false, false, false, false, false, true, false};

const uint8_t SYMBOL_SIZE = 9;
const uint8_t BLOCK_SIZE = 8;

class InvalidCommand : public std::exception {
public:
    std::string Errortype();
};

class EmptyContainer : public std::exception {
public:
    std::string Errortype();
};