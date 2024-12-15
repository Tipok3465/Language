#pragma once

#include "vector"
#include "iostream"
#include "variant"

#include "Lexeme.h"

class Generator {
public:
    Generator() : cur_(0) {}
    enum class PolizType {
        FuncEnd, FuncStart, Semicolon, Return, SwitchCmp,
        Scan, Print, Comma, GoToAnyway, GoToFalse
    };
    enum class GenType {
        Identifier, Operation, Literal,
        Address, Function
    };
    void pushStack(Lexeme lex);
    void pushPoliz(Lexeme lex);
    void clearStack();
    int getPos();
    void setPos(int i, int x);

    std::vector<Lexeme> getPoliz();

private:
    int cur_;
    std::vector<Lexeme> stack_;
    std::vector<Lexeme> poliz_;
    std::vector<LexemeType> types_;
};
