//
// Created by Egor on 22.10.2024.
//

#ifndef LANGUAGE_BOR_H
#define LANGUAGE_BOR_H

#include <string>
#include <map>

#include "Lexeme.h"

class Bor {
public:
    Bor();
    ~Bor();
    void add(const std::string& s);
    bool check(const std::string& s) const;
private:
    struct Node {
        std::map<char, Node*> to;
        bool isTerminal = false;
    };

    void clear(Node* node);

    Node* start;
};


#endif //LANGUAGE_BOR_H
