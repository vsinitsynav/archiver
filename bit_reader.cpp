#include "bit_reader.h"

BitReader::BitReader(std::istream& stream, bool is_little_endian)
    : buffer_(0), bits_remain_(0), stream_(stream), eof_(false), is_little_endian_(is_little_endian) {
}

std::vector<bool> BitReader::ReadBits(size_t count) {
    std::vector<bool> result;
    for (size_t i = 0; i < count; ++i) {
        if (bits_remain_ == 0) {
            buffer_ = 0;
            bits_remain_ = BLOCK_SIZE;
            stream_.get(reinterpret_cast<char&>(buffer_));
            if (stream_.eof()) {
                buffer_ = 0;
                bits_remain_ = BLOCK_SIZE;
                eof_ = true;
            }
        }
        result.push_back((buffer_ >> (bits_remain_ - 1)) & 1);
        bits_remain_ -= 1;
    }
    if (is_little_endian_) {
        std::reverse(result.begin(), result.end());
    }
    return result;
}

bool BitReader::Eof() const {
    return eof_;
}

void BitReader::ResetStream() {
    stream_.clear();
    stream_.seekg(0);
    eof_ = false;
    buffer_ = 0;
    bits_remain_ = 0;
}