
#include "huffman.h"
#include "constants.h"

Node::Node() {
    tl = nullptr;
    tr = nullptr;
    is_term = false;
    character = {};
}

Node::Node(BitSymbol symb) {
    tl = nullptr;
    tr = nullptr;
    is_term = true;
    character = symb;
}

Node::Node(Node* node1, Node* node2) {
    tl = node1;
    tr = node2;

    if (node1->character < node2->character) {
        character = node1->character;
    } else {
        character = node2->character;
    }
    is_term = false;
}

void Trie::AddCode(BitSymbol charecter, Node* vertex, std::vector<bool> code, size_t pos) {
    if (pos == code.size()) {
        return;
    }

    if (code[pos] == 0) {
        if (vertex->tl == nullptr) {
            if (pos + 1 == code.size()) {
                to_delete.push_back(new Node(charecter));
            } else {
                to_delete.push_back(new Node());
            }
            vertex->tl = to_delete.back();
        }
        AddCode(charecter, vertex->tl, code, pos + 1);
    } else {
        if (vertex->tr == nullptr) {
            if (pos + 1 == code.size()) {
                to_delete.push_back(new Node(charecter));
            } else {
                to_delete.push_back(new Node());
            }
            vertex->tr = to_delete.back();
        }
        AddCode(charecter, vertex->tr, code, pos + 1);
    }
}

Trie::Trie(Node* vroot) {
    root = vroot;
}

Trie::Trie() {
}

Trie::Trie(const std::vector<std::pair<BitSymbol, std::vector<bool>>>& vec) {
    to_delete.push_back(new Node());
    root = to_delete.back();
    for (const auto& [character, code] : vec) {
        AddCode(character, root, code, 0);
    }
}

void Trie::Tour(Node* vertex, std::vector<std::pair<BitSymbol, uint64_t>>& vec, uint64_t height) {
    if (vertex->is_term == true) {
        vec.push_back({vertex->character, height});
        return;
    }

    if (vertex->tl != nullptr) {
        Tour(vertex->tl, vec, height + 1);
    }

    if (vertex->tr != nullptr) {
        Tour(vertex->tr, vec, height + 1);
    }
}

std::pair<uint64_t, Node*> Huffman::GetFront(std::queue<std::pair<uint64_t, Node*>>& queue1,
                                             std::queue<std::pair<uint64_t, Node*>>& queue2) {
    if (queue1.empty()) {
        std::pair<uint64_t, Node*> to_return = queue2.front();
        queue2.pop();
        return to_return;
    }

    if (queue2.empty()) {
        std::pair<uint64_t, Node*> to_return = queue1.front();
        queue1.pop();
        return to_return;
    }
    assert(queue1.front().second->character.size() == SYMBOL_SIZE);
    assert(queue2.front().second->character.size() == SYMBOL_SIZE);
    if ((queue1.front().first == queue2.front().first &&
         queue1.front().second->character < queue2.front().second->character) ||
        queue1.front().first < queue2.front().first) {
        std::pair<uint64_t, Node*> to_return = queue1.front();
        queue1.pop();
        return to_return;
    } else {
        std::pair<uint64_t, Node*> to_return = queue2.front();
        queue2.pop();
        return to_return;
    }
}

Huffman::Huffman(const std::vector<std::pair<BitSymbol, uint64_t>>& length) {
    cnt_of_length.resize(length.back().second + 1);

    std::vector<bool> code;

    trie.to_delete.push_back(new Node());
    trie.root = trie.to_delete.back();

    for (const auto& [character, length] : length) {
        ++cnt_of_length[length];
        while (code.size() < length) {
            code.push_back(false);
        }
        symbols_code[character] = code;
        trie.AddCode(character, trie.root, code, 0);
        while (!code.empty() && code.back() == 1) {
            code.pop_back();
        }
        if (!code.empty()) {
            code.pop_back();
            code.push_back(true);
        }
        while (code.size() < length) {
            code.push_back(false);
        }
    }

    root = trie.root;
    current = trie.root;
}

Huffman::Huffman(const std::unordered_map<BitSymbol, uint64_t>& char_frequency) {
    char_frequency_ = char_frequency;

    try {
        std::vector<std::pair<uint64_t, BitSymbol>> order;
        for (const auto& [character, frequency] : this->char_frequency_) {
            order.emplace_back(frequency, character);
        }

        if (order.empty()) {
            throw EmptyContainer();
        }

        std::sort(order.begin(), order.end());

        std::queue<std::pair<uint64_t, Node*>> queue1;
        std::queue<std::pair<uint64_t, Node*>> queue2;

        for (const auto& [frequency, character] : order) {
            trie.to_delete.push_back(new Node(character));
            queue1.push(std::make_pair(frequency, trie.to_delete.back()));
        }
        while (!queue1.empty() || queue2.size() > 1) {
            auto [frequency1, node1] = GetFront(queue1, queue2);
            auto [frequency2, node2] = GetFront(queue1, queue2);

            trie.to_delete.push_back(new Node(node1, node2));
            queue2.push({frequency1 + frequency2, trie.to_delete.back()});
        }

        if (queue2.empty()) {
            throw EmptyContainer();
        }

        trie.root = queue2.front().second;

        // НА ЭТОМ ЭТАПЕ ПОСТРОЕН БОР, ОСТАЛОСЬ ПРИВЕСТИ ЕГО К КАНОНИЧЕСКОМУ ВИДУ

        std::vector<std::pair<BitSymbol, uint64_t>> code_length;

        trie.Tour(trie.root, code_length, 0);

        std::sort(code_length.begin(), code_length.end(),
                  [](const std::pair<BitSymbol, uint64_t>& char1, const std::pair<BitSymbol, uint64_t>& char2) {
                      if (char1.second != char2.second) {
                          return char1.second < char2.second;
                      } else {
                          return char1.first < char2.first;
                      }
                  });

        cnt_of_length.resize(code_length.back().second + 1);

        // Теперь есть посорченый список: символ -- длина кода
        // Дальше приводим коды к каноническому виду

        std::vector<bool> code;
        for (const auto& [character, length] : code_length) {
            ++cnt_of_length[length];
            while (code.size() < length) {
                code.push_back(false);
            }
            symbols_code[character] = code;
            while (!code.empty() && code.back() == 1) {
                code.pop_back();
            }
            if (!code.empty()) {
                code.pop_back();
                code.push_back(true);
            }
            while (code.size() < length) {
                code.push_back(false);
            }
        }

    } catch (const EmptyContainer& e) {
        std::cerr << "Char_frequency table is empty\n";
        std::cerr << e.what() << "\n";
    }
}

void Trie::Delete() {
    for (auto& node : to_delete) {
        delete node;
        node = nullptr;
    }
}