//
// Created by Данил Подлягин on 22.10.2024.
//

#include <iostream>
#include "Translator.h"
#include "LexicalAnalyzer.h"

void Translator::run() {
    LexicalAnalyzer analyzer;
    Lexeme lexeme = analyzer.getLexeme();
    while (lexeme.getType() != LexemeType::EndOfFile) {
        std::cout << "Lexeme: " << lexeme.getName() << '\n' << "Id: " << lexeme.getId() << '\n';
        lexeme = analyzer.getLexeme();
    }
    std::cout << "All is OK\n";
}