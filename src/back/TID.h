#pragma once

#include <map>

#include "Lexeme.h"
#include "Error.h"

class TID {
public:
    TID() = default;
    void pushID(std::string type, std::string id) {
        if (data_[id] != "") throw Error("Double definition: " + id);
        data_[id] = type;
    }
    bool checkID(std::string id) {
        return data_[id] != "";
    }
private:
    std::map<std::string, std::string> data_;
};