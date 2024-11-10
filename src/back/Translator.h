//
// Created by Данил Подлягин on 22.10.2024.
//
#pragma once


#include "LexicalAnalyzer.h"
#include "iostream"

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
    void nextState();
};

