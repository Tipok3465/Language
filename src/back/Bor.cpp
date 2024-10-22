//
// Created by Egor on 22.10.2024.
//

#include "Bor.h"

Bor::Bor() : start(new Node()) {}

Bor::~Bor() {
    clear(start);
}

void Bor::clear(Bor::Node *node) {
    for (auto [c, child] : node->to) {
        if (child != nullptr) clear(child);
    }
    delete node;
}

void Bor::add(const std::string& s) {
    Node *node = start;
    for (char c : s) {
        if (node->to[c] == nullptr) node->to[c] = new Node();
        node = node->to[c];
    }
    node->isTerminal = true;
}

bool Bor::check(const std::string& s) const {
    Node *node = start;
    for (char c : s) {
        if (node->to[c] == nullptr) return false;
        node = node->to[c];
    }
    return node->isTerminal;
}