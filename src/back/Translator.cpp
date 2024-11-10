//
// Created by Данил Подлягин on 22.10.2024.
//

#include <iostream>
#include "Translator.h"
#include "LexicalAnalyzer.h"

void Translator::run() {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    while (lexeme.getType() != LexemeType::EndOfFile) {
        std::cout << "\e[1;32mLexeme: \e[1;33m" << lexeme.getName() << "\e[0m    ";
        std::cout << "\e[1;32mType: \e[0m" << lexeme.toString() << "    ";
        std::cout << "\e[1;32mLine: \e[0m" << lexeme.getId() << '\n';
        std::cout << "-------------------------------------\n";
        lexeme = lex_analyzer_.getLexeme();
    }
    std::cout << "All is OK\n";
}

void Translator::update() {
    activeState();
}

void Translator::startState() {
    std::cout << "START" << std::endl;
    activeState = [&]() { nextState(); };
}

void Translator::nextState() {
    std::cout << "NEXT" << std::endl;
}