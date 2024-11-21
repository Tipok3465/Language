//
// Created by Egor on 18.11.2024.
//

#include "NameChecker.h"

#include <fstream>

void NameChecker::pushId(std::string type, std::string id) {
    cur_->tid.pushID(type, id);
}

bool NameChecker::checkIdRec(NameChecker::Node *node, std::string id) {
    if (node == nullptr) return false;
    if (node->tid.checkID(id)) return true;
    return checkIdRec(node->p, id);
}

bool NameChecker::checkId(std::string id) {
    return checkIdRec(cur_, id);
}

NameChecker::NameChecker() {
    cur_ = new Node;
}

NameChecker::~NameChecker() {
    while (cur_->p != nullptr) cur_ = cur_->p;
    clear(cur_);
}

void NameChecker::clear(NameChecker::Node *node) {
    for (Node *child : node->children) clear(child);
    delete node;
}

void NameChecker::createScope() {
    cur_->children.push_back(new Node);
    cur_->children.back()->p = cur_;
    cur_ = cur_->children.back();
}

void NameChecker::exitScope() {
    cur_ = cur_->p;
}

std::string NameChecker::getType(std::string id) {
    if (!checkId(id)) throw Error("Undefined variable " + id);
    Node* pointer = cur_;
    while (!pointer->tid.checkID(id)) pointer = pointer->p;
    return pointer->tid.getType(id);
}