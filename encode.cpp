#include "constants.h"
#include "bit_reader.h"
#include "bit_writer.h"
#include "huffman.h"


BitSymbol BuildBitSymbol(uint64_t symbol) {
    BitSymbol bit_symbol(SYMBOL_SIZE);
    for (int i = 0; i < SYMBOL_SIZE; ++i) {
        bit_symbol[i] = ((symbol >> (SYMBOL_SIZE - i - 1)) & 1);
    }
    return bit_symbol;
}

void WriteBitSequence(std::ofstream& ostream, BitSymbol symbol, uint64_t& buffer_size, uint64_t& buffer) {
    for (size_t i = 0; i < symbol.size(); ++i) {
        bool bit = symbol[i];
        buffer <<= 1;
        if (bit) {
            buffer |= 1;
        }
        ++buffer_size;
        if (buffer_size == BLOCK_SIZE) {
            ostream.write(reinterpret_cast<const char*>(&buffer), sizeof(uint8_t));
            buffer = 0;
            buffer_size = 0;
        }
    }
}

void WriteNumber(std::ofstream& ostream, uint64_t number, uint64_t& buffer_size, uint64_t& buffer) {
    WriteBitSequence(ostream, BuildBitSymbol(number), buffer_size, buffer);
}

void TryPrint(std::ofstream& ostream, uint64_t& buffer_size, uint64_t& buffer) {
    if (buffer_size) {
        buffer <<= (BLOCK_SIZE - buffer_size);
        ostream.write(reinterpret_cast<const char*>(&buffer), sizeof(uint8_t));
        buffer = 0;
        buffer_size = 0;
    }
}

void EncodeFile(std::ofstream& ostream, const std::string& filepath, bool is_last_file, uint64_t& buffer_size,
                uint64_t& buffer) {
    // Считывание файла
    size_t slash_index = filepath.rfind('/');
    std::string filename = filepath.substr((slash_index == std::string::npos ? 0 : slash_index + 1));
    std::ifstream istream(filename, std::ios::binary);
    BitReader reader(istream, false);

    std::unordered_map<BitSymbol, uint64_t> char_frequency;

    std::vector<BitSymbol> filename_symbols;

    // Символы из названия файла в последовательность по 9 бит
    for (uint64_t symbol : filename) {
        BitSymbol bit_symbol(SYMBOL_SIZE);
        for (int i = 1; i < SYMBOL_SIZE; ++i) {
            bit_symbol[i] = ((symbol >> (SYMBOL_SIZE - i - 1)) & 1);
        }
        filename_symbols.push_back(bit_symbol);
        ++char_frequency[bit_symbol];
    }

    while (!reader.Eof()) {
        // Теперь считываем по символу из файла и добавляем в хешмапу частот
        BitSymbol symbol = reader.ReadBits(BLOCK_SIZE);
        symbol.insert(symbol.begin(), false);

        if (reader.Eof()) {
            break;
        }

        ++char_frequency[symbol];
    }

    char_frequency[FILENAME_END] = 1;
    char_frequency[ONE_MORE_FILE] = 1;
    char_frequency[ARCHIVE_END] = 1;

    // К этому моменту прошлась по файлу и выписала частоты всех символов

    Huffman huffman(char_frequency);

    for (const auto& x : huffman.symbols_code) {
        BitSymbol symbol = x.first;
        BitSymbol code = x.second;
    }

    // Теперь записываем в архив описание бора

    WriteNumber(ostream, char_frequency.size(), buffer_size, buffer);
    std::vector<std::pair<std::vector<bool>, BitSymbol>> coding;

    for (const auto& [symbol, code] : huffman.symbols_code) {
        coding.emplace_back(code, symbol);
    }

    std::sort(coding.begin(), coding.end());

    for (const auto& [code, symbol] : coding) {
        WriteBitSequence(ostream, symbol, buffer_size, buffer);
    }

    std::unordered_map<size_t, size_t> symbol_size_count;

    for (size_t i = 1; i < huffman.cnt_of_length.size(); ++i) {
        WriteNumber(ostream, huffman.cnt_of_length[i], buffer_size, buffer);
    }
    for (const BitSymbol& symbol : filename_symbols) {
        WriteBitSequence(ostream, huffman.symbols_code[symbol], buffer_size, buffer);
    }

    WriteBitSequence(ostream, huffman.symbols_code[FILENAME_END], buffer_size, buffer);

    // Осталось только закодировать содержимое файла

    reader.ResetStream();
    while (!reader.Eof()) {
        BitSymbol symbol = reader.ReadBits(BLOCK_SIZE);
        symbol.insert(symbol.begin(), false);

        if (reader.Eof()) {
            break;
        }
        WriteBitSequence(ostream, huffman.symbols_code[symbol], buffer_size, buffer);
    }

    if (is_last_file) {
        WriteBitSequence(ostream, huffman.symbols_code[ARCHIVE_END], buffer_size, buffer);
    } else {
        WriteBitSequence(ostream, huffman.symbols_code[ONE_MORE_FILE], buffer_size, buffer);
    }

    for (auto& node : huffman.trie.to_delete) {
        delete node;
        node = nullptr;
    }
}

void EncodeFiles(const std::string& output_file, const std::vector<std::string>& filepaths) {
    std::ofstream ostream(output_file, std::ios::out | std::ios::binary);
    uint64_t buffer_size = 0;
    uint64_t buffer = 0;

    for (size_t i = 0; i < filepaths.size(); ++i) {
        std::string filepath = filepaths[i];
        bool is_last = (i + 1 == filepaths.size());
        EncodeFile(ostream, filepath, is_last, buffer_size, buffer);
    }
    TryPrint(ostream, buffer_size, buffer);

    ostream.close();
}