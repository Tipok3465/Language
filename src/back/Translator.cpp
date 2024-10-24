//
// Created by Данил Подлягин on 22.10.2024.
//

#include <iostream>
#include "Translator.h"
#include "LexicalAnalyzer.h"

void Translator::run() {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    while (lexeme.getType() != LexemeType::EndOfFile) {
        std::cout << "Lexeme: " << lexeme.getName() << "    ";
        std::cout << "Type: " << lexeme.toString() << "    ";
        std::cout << "Line: " << lexeme.getId() << '\n';
        std::cout << "-------------------------------------\n";
        lexeme = lex_analyzer_.getLexeme();
    }
    std::cout << "All is OK\n";
}