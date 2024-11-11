//
// Created by Данил Подлягин on 22.10.2024.
//

#include <iostream>
#include "Translator.h"
#include "LexicalAnalyzer.h"

void Translator::run() {
//    Lexeme lexeme = lex_analyzer_.getLexeme();
//    while (lexeme.getType() != LexemeType::EndOfFile) {
//        std::cout << "\e[1;32mLexeme: \e[1;33m" << lexeme.getName() << "\e[0m    ";
//        std::cout << "\e[1;32mType: \e[0m" << lexeme.toString() << "    ";
//        std::cout << "\e[1;32mLine: \e[0m" << lexeme.getId() << '\n';
//        std::cout << "-------------------------------------\n";
//        lexeme = lex_analyzer_.getLexeme();
//    }
    startScanning();
    std::cout << "All is OK\n";
}

bool Translator::isType(Lexeme lexeme) {
    return lexeme.getName() == "integer" || lexeme.getName() == "float" || lexeme.getName() == "bool" ||
           lexeme.getName() == "char" || lexeme.getName() == "void" && lexeme.getType() == LexemeType::Service;
}

void Translator::startScanning() {
    try {
        startState();
    } catch (Lexeme lex) {
        if (lex.getType() == LexemeType::Error) {
            std::cerr << "Unknown lexeme: " + lex.getName() << '\n';
            std::cerr << lex.getId() << " string\n";
        } else {
            std::cerr << "Lexeme in wrong place: " + lex.getName() << '\n';
            std::cerr << "#" << lex.getId() << " string\n";
        }
    }
}

void Translator::startState() {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::Service) {
        if (lexeme.getName() == "Main" && lexeme.getType() == LexemeType::Service) mainState();
        if (isType(lexeme)) definitionState(lexeme.getName());
        if (lexeme.getName() == "const" && lexeme.getType() == LexemeType::Service) constDefinitionState();
    }
    if (lexeme.getType() == LexemeType::Error) throw lexeme;

}

void Translator::constDefinitionState() {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() != LexemeType::Service || !isType(lexeme)) throw lexeme;
    lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() != LexemeType::Identifier) throw lexeme;
    lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::EndOfLine) {
        startState();
        return;
    }
    if (lexeme.getType() == LexemeType::Operator && lexeme.getName() == "=") {
        calculationState();
    }
    lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::EndOfLine) {
        startState();
        return;
    }
    throw lexeme;
}

void Translator::definitionState(std::string type) {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::Operator && lexeme.getName() == "<-") functionDefinitionState(type);
    else if (lexeme.getType() == LexemeType::Identifier) variableInitializationState(type, lexeme.getName());
    else throw lexeme;
}

void Translator::functionDefinitionState(std::string type) {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() != LexemeType::Identifier) throw lexeme;
    lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() != LexemeType::OpenBrace) throw lexeme;
    lexeme = lex_analyzer_.getLexeme();
    while (lexeme.getType() != LexemeType::CloseBrace) {
        if (!isType(lexeme)) throw lexeme;
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::Identifier) throw lexeme;
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::CloseBrace && lexeme.getType() != LexemeType::Comma) throw lexeme;
    }
    lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::Brace && lexeme.getName() == "{") {
        lex_analyzer_.getBack(lexeme);
        blockState();
        return;
    }
    lex_analyzer_.getBack(lexeme);
    operatorState();
}

void Translator::variableInitializationState(std::string type, std::string identifier) {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::EndOfLine) {
        startState();
        return;
    }
    if (lexeme.getType() == LexemeType::Operator && lexeme.getName() == "=") {
        calculationState();
    }
    lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::EndOfLine) {
        startState();
        return;
    }
    throw lexeme;
}

void Translator::mainState() {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::Brace && lexeme.getName() == "{") {
        lex_analyzer_.getBack(lexeme);
        blockState();
        return;
    }
    lex_analyzer_.getBack(lexeme);
    operatorState();
}

void Translator::calculationState() {
    calc8State();
    Lexeme lexeme = lex_analyzer_.getLexeme();
    while (lexeme.getName() == "," && lexeme.getType() == LexemeType::Comma) {
        calc8State();
        lexeme = lex_analyzer_.getLexeme();
    }
    lex_analyzer_.getBack(lexeme);
}

void Translator::calc8State() {
    calc7State();
    Lexeme lexeme = lex_analyzer_.getLexeme();
    while (lexeme.getName() == "or" && lexeme.getType() == LexemeType::Operator) {
        calc7State();
        lexeme = lex_analyzer_.getLexeme();
    }
    lex_analyzer_.getBack(lexeme);
}

void Translator::calc7State() {
    calc6State();
    Lexeme lexeme = lex_analyzer_.getLexeme();
    while (lexeme.getName() == "and" && lexeme.getType() == LexemeType::Operator) {
        calc6State();
        lexeme = lex_analyzer_.getLexeme();
    }
    lex_analyzer_.getBack(lexeme);
}

void Translator::calc6State() {
    calc5State();
    Lexeme lexeme = lex_analyzer_.getLexeme();
    while (lexeme.getName() == "==" || lexeme.getName() == "!=" && lexeme.getType() == LexemeType::Operator) {
        calc5State();
        lexeme = lex_analyzer_.getLexeme();
    }
    lex_analyzer_.getBack(lexeme);
}

void Translator::calc5State() {
    calc4State();
    Lexeme lexeme = lex_analyzer_.getLexeme();
    while (lexeme.getType() == LexemeType::Operator &&
           lexeme.getName() == "<" || lexeme.getName() == "<=" ||
           lexeme.getName() == ">" || lexeme.getName() == ">=") {
        calc4State();
        lexeme = lex_analyzer_.getLexeme();
    }
    lex_analyzer_.getBack(lexeme);
}

void Translator::calc4State() {
    calc3State();
    Lexeme lexeme = lex_analyzer_.getLexeme();
    while (lexeme.getType() == LexemeType::Operator &&
           lexeme.getName() == "+" || lexeme.getName() == "-") {
        calc3State();
        lexeme = lex_analyzer_.getLexeme();
    }
    lex_analyzer_.getBack(lexeme);
}

void Translator::calc3State() {
    calc2State();
    Lexeme lexeme = lex_analyzer_.getLexeme();
    while (lexeme.getType() == LexemeType::Operator &&
           lexeme.getName() == "*" || lexeme.getName() == "/" || lexeme.getName() == "%") {
        calc2State();
        lexeme = lex_analyzer_.getLexeme();
    }
    lex_analyzer_.getBack(lexeme);
}

void Translator::calc2State() {
    calc1State();
    Lexeme lexeme = lex_analyzer_.getLexeme();
    while (lexeme.getType() == LexemeType::Operator &&
           lexeme.getName() == "++" || lexeme.getName() == "--") {
        calc1State();
        lexeme = lex_analyzer_.getLexeme();
    }
    lex_analyzer_.getBack(lexeme);
}

void Translator::calc1State() {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::OpenBrace) {
        calculationState();
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::CloseBrace) throw lexeme;
        return;
    }
    if (lexeme.getType() == LexemeType::Literal) return;
    if (lexeme.getType() == LexemeType::Identifier) {
        Lexeme pref_lex = lexeme;
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() == LexemeType::OpenBrace) {
            lex_analyzer_.getBack(pref_lex);
            lex_analyzer_.getBack(lexeme);
            functionCallState();
            return;
        }
        if (lexeme.getType() == LexemeType::Dot) {
            functionCallState();
            return;
        }
        if (lexeme.getType() == LexemeType::SquareBrace && lexeme.getName() == "[") {
            calculationState();
            lexeme = lex_analyzer_.getLexeme();
            if (lexeme.getType() != LexemeType::SquareBrace || lexeme.getName() != "]") throw lexeme;
            return;
        }
        lex_analyzer_.getBack(lexeme);
        return;
    }
}

void Translator::functionCallState() {

}

void Translator::blockState() {

}

void Translator::operatorState() {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getName() == "if" && lexeme.getType() == LexemeType::Service) {
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::OpenBrace) throw lexeme;
        calculationState();
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::CloseBrace) throw lexeme;
        Lexeme lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() == LexemeType::Brace && lexeme.getName() == "{") {
            lex_analyzer_.getBack(lexeme);
            blockState();
            return;
        }
        lex_analyzer_.getBack(lexeme);
        operatorState();
    }

}