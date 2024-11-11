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
    int x;
}

bool Translator::isType(Lexeme lexeme) {
    return lexeme.getName() == "integer" || lexeme.getName() == "float" || lexeme.getName() == "bool" ||
           lexeme.getName() == "char" || lexeme.getName() == "void";
}

void Translator::update() {
    activeState();
}

void Translator::startState() {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::Service) {
        if (lexeme.getName() == "Main") activeState = [&]() { mainState(); };
        if (isType(lexeme)) activeState = [&]() { definitionState(lexeme.getName()); };
        if (lexeme.getName() == "const") activeState = [&]() { constDefinitionState(); };
    }
}

void Translator::constDefinitionState() {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::Service && isType(lexeme)) activeState = [&]() { definitionState(lexeme.getName()); };
    else throw lexeme;
}

void Translator::definitionState(std::string type) {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::Service && lexeme.getName() == "<-") activeState = [&]() { functionDefinitionState(type); };
    else if (lexeme.getType() == LexemeType::Identifier) activeState = [&]() { variableInitializationState(type, lexeme.getName()); };
    else throw lexeme;
}

void Translator::functionDefinitionState(std::string type) {

}

void Translator::variableInitializationState(std::string type, std::string identifier) {

}

void Translator::mainState() {

}