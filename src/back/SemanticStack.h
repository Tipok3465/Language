#pragma once

#include <vector>

#include "Lexeme.h"
#include "Error.h"
#include "NameChecker.h"

class SemanticStack {
public:
    SemanticStack(NameChecker& varChecker);
    void push(Lexeme lexeme) {
        data_.push_back(lexeme);
    }
    void checkUno();
    void checkBin();
    void clear();
    Lexeme pop();
private:
    std::vector<Lexeme> data_;
    NameChecker& varChecker_;

};
