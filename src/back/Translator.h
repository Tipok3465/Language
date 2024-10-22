//
// Created by Данил Подлягин on 22.10.2024.
//
#pragma once


#include "LexicalAnalyzer.h"

#include "iostream"

class Translator {
public:
    Translator() = default;
    void run();
private:
    LexicalAnalyzer lex_analyzer_;
};

