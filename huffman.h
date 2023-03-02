// BEGINNOLINT

#pragma once

#include "constants.h"

class Node {
public:
    Node();

    explicit Node(BitSymbol symbol);

    Node(Node* node1, Node* node2);

    bool is_term;
    BitSymbol character;

    Node* tl = nullptr;
    Node* tr = nullptr;

    //~Node();
};

class Trie {
public:
    Trie();

    explicit Trie(Node* vroot);

    explicit Trie(const std::vector<std::pair<BitSymbol, std::vector<bool>>>& vec);

    void Tour(Node* vertex, std::vector<std::pair<BitSymbol, uint64_t>>& vec, uint64_t height);

    void AddCode(BitSymbol character, Node* vertex, std::vector<bool> code, size_t pos);

    Node* root;

    void Delete();

    //~Trie();
    std::vector<Node*> to_delete;
};

class Huffman {
public:
    explicit Huffman(const std::unordered_map<BitSymbol, uint64_t>& char_frequency);

    explicit Huffman(const std::vector<std::pair<BitSymbol, uint64_t>>& length);

    void DeleteNodes(Node* node);

    std::pair<uint64_t, Node*> GetFront(std::queue<std::pair<uint64_t, Node*>>& queue1,
                                        std::queue<std::pair<uint64_t, Node*>>& queue2);

    std::unordered_map<BitSymbol, uint64_t> char_frequency_;

    std::unordered_map<BitSymbol, std::vector<bool>> symbols_code;

    void ToCanonicalCode();

    int64_t max_length;

    std::vector<int64_t> cnt_of_length;

    //~Huffman();

    Trie trie;

    Node* root;
    Node* current;
};

// ENDNOLINT