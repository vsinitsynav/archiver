#pragma once

#include "constants.h"

class BitReader {
public:
    BitReader(std::istream& stream, bool is_little_endian);

    std::vector<bool> ReadBits(size_t count);

    void ResetStream();

    bool Eof() const; 
private:
    uint8_t buffer_;
    uint8_t bits_remain_;

    std::istream& stream_;

    bool eof_;
    bool is_little_endian_;
};