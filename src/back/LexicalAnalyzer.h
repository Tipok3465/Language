//
// Created by Данил Подлягин on 22.10.2024.
//

#pragma once

#include "Lexeme.h"
#include "Bor.h"

#include "fstream"
#include "iostream"
#include <string>
#include <stack>

class LexicalAnalyzer {
public:
    LexicalAnalyzer();
    ~LexicalAnalyzer();
    Lexeme getLexeme();
    void getBack(Lexeme lex);
private:
    char* code_text_;
    int size_;
    std::stack<Lexeme> pref_lexemes_;
    Bor serviceBor_;
    Bor operatorBor_;
    std::ifstream fin;
    int cur_;
    int id_;
    int line_;

    void createServiceBor();
    void createOperatorBor();
    char getSymbol();
    bool contains(int leftBorder, int rightBorder, int val);
};


