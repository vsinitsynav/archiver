#include "bit_writer.h"

BitWriter::BitWriter(std::ostream& stream) : buffer_(0), bits_(0), stream_(stream) {
}

void BitWriter::WriteBits(std::vector<bool> symbol_to_write) {
    for (size_t i = 0; i < symbol_to_write.size(); ++i) {
        bool bit = symbol_to_write[i];
        buffer_ <<= 1;
        if (bit) {
            buffer_ |= 1;
        }
        bits_++;
        if (bits_ == BLOCK_SIZE) {
            stream_.write(reinterpret_cast<const char*>(&buffer_), sizeof(uint8_t));
            buffer_ = 0;
            bits_ = 0;
        }
    }
}

void BitWriter::Close() {
    if (bits_ > 0) {
        buffer_ <<= (BLOCK_SIZE - bits_);
        stream_.write(reinterpret_cast<const char*>(&buffer_), sizeof(uint8_t));
    }
}

void BitWriter::WriteNumber(size_t data, size_t bits, bool is_little_endian) {
    std::vector<bool> bit(bits, false);
    for (int i = 0; i < bits; ++i) {
        bit[i] = ((data >> (bits - i - 1)) & 1);
    }
    WriteBits(bit);
}
