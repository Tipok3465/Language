//
// Created by Egor on 18.11.2024.
//

#ifndef LANGUAGE_NAMECHECKER_H
#define LANGUAGE_NAMECHECKER_H

#include "TID.h"
#include <vector>

class NameChecker {
public:
    NameChecker();

    ~NameChecker();

    bool checkId(std::string id);

    void pushId(std::string type, std::string id);

    std::string getType(std::string id);

    void createScope();

    void exitScope();

private:
    struct Node {
        TID tid = TID();
        std::vector<Node *> children;
        Node *p = nullptr;
    };

    bool checkIdRec(Node *node, std::string id);

    void clear(Node *node);

    Node *cur_;
};


#endif //LANGUAGE_NAMECHECKER_H
