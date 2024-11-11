//
// Created by Данил Подлягин on 22.10.2024.
//
#pragma once


#include "LexicalAnalyzer.h"
#include "iostream"
#include "functional"

class Translator {
public:
    Translator() {
        activeState = [&]() { startState(); };
    }
    void run();
    void update();
private:
    LexicalAnalyzer lex_analyzer_;
    std::function<void()> activeState;
    void startState();
    void mainState();
    void definitionState(std::string type);
    void constDefinitionState();
    void functionDefinitionState(std::string type);
    void variableInitializationState(std::string type, std::string identifier);

    bool isType(Lexeme lexeme);
};

