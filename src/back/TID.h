#pragma once

#include <map>

#include "Lexeme.h"

enum class Error {
    DoubleDefinition
};

class TID {
public:
    TID() = default;
    void pushID(std::string type, std::string id) {
        if (!data_[id].empty()) throw Error::DoubleDefinition;
        data_[id] = type;
    }
    bool checkID(std::string id) {
        return !data_[id].empty();
    }
private:
    std::map<std::string, std::string> data_;
};