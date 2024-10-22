//
// Created by Данил Подлягин on 22.10.2024.
//

#pragma once

#include "Lexeme.h"

#include "fstream"
#include "iostream"
#include <string>

class LexicalAnalyzer {
public:
    LexicalAnalyzer();
private:
    char* code_text_;
};

