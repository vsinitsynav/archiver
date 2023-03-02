#pragma once

#include "constants.h"

class BitWriter {
public:
    explicit BitWriter(std::ostream& stream);

    void WriteBits(std::vector<bool> symbol_to_write);

    void WriteNumber(size_t data, size_t bits, bool is_little_endian);

    void Close();

private:
    uint8_t buffer_;
    uint8_t bits_;

    std::ostream& stream_;
};