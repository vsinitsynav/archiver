#include "constants.h"
#include "bit_reader.h"
#include "bit_writer.h"
#include "huffman.h"

size_t BitsetToNumber(const std::vector<bool>& a) {
    size_t res = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i]) {
            res |= (1 << (a.size() - 1 - i));
        }
    }
    return res;
}

void DecodeFile(const std::string& filepath, const std::vector<std::string>& file_paths) {
    std::ifstream istream(filepath, std::ios::binary);

    if (!istream.is_open() || istream.bad()) {
        throw new std::runtime_error("File not found");
    }
    BitReader reader(istream, false);
    bool archive_end = false;

    while (!archive_end) {

        // Декодируем последовательность символов для канонического вида

        size_t symbol_count = BitsetToNumber(reader.ReadBits(SYMBOL_SIZE));
        std::cout << "SYMBOLS COUNT " << symbol_count << "\n";

        std::vector<BitSymbol> symbols;
        for (size_t i = 0; i < symbol_count; ++i) {
            symbols.push_back(reader.ReadBits(SYMBOL_SIZE));
        }

        std::vector<std::pair<BitSymbol, uint64_t>> lengths;
        size_t symbol_index = 0;
        size_t length = 0;
        size_t current_count = 0;

        // Сопостовляем длины кодов символам

        while (symbol_index < symbol_count) {
            while (current_count == 0) {
                current_count = BitsetToNumber(reader.ReadBits(SYMBOL_SIZE));
                ++length;
            }
            lengths.emplace_back(symbols[symbol_index], length);
            ++symbol_index;
            --current_count;
        }

        Huffman huffman(lengths);

        // Декодируем название файла

        std::string output_filename;
        while (true) {
            assert(huffman.current != nullptr);
            while (huffman.current->is_term == false) {
                Node* prev = huffman.current;
                bool bit = reader.ReadBits(1)[0];
                if (bit) {
                    huffman.current = huffman.current->tr;
                } else {
                    huffman.current = huffman.current->tl;
                }
                assert(huffman.current != prev);
            }
            assert(huffman.current != nullptr);
            if (huffman.current->character != FILENAME_END) {
                output_filename.push_back(static_cast<char>(BitsetToNumber(huffman.current->character)));
                huffman.current = huffman.root;
            } else {
                break;
            }
        }

        std::cout << "FILE NAME WAS DECOMPRESSED: " << output_filename << " \n";

        std::ofstream ostream(output_filename, std::ios::binary);

        if (!ostream.is_open() || ostream.bad()) {
            throw new std::runtime_error("Cannot open file to write");
        }
        BitWriter writer(ostream);
        huffman.current = huffman.root;
        std::vector<bool> path;
        while (true) {
            while (huffman.current->is_term == false) {
                assert(huffman.current != nullptr);
                bool bit = reader.ReadBits(1)[0];
                path.push_back(bit);
                if (bit) {
                    huffman.current = huffman.current->tr;
                } else {
                    huffman.current = huffman.current->tl;
                }
            }
            if (huffman.current->character != ARCHIVE_END && huffman.current->character != ONE_MORE_FILE) {
                BitSymbol code = huffman.current->character;
                path.clear();
                code.erase(code.begin());
                writer.WriteBits(code);
                huffman.current = huffman.root;
            } else {
                break;
            }
        }
        writer.Close();
        if (huffman.current->character == ARCHIVE_END) {
            archive_end = true;
        }
        for (auto& node : huffman.trie.to_delete) {
            delete node;
            node = nullptr;
        }
    }
}