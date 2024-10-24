//
// Created by Данил Подлягин on 22.10.2024.
//
#pragma once
#include <utility>

#include "string"

enum class LexemeType {
    Service,
    Identifier,
    Literal,
    Operator,
    Comma,
    Dot,
    Brace,
    SquareBrace,
    OpenBrace,
    CloseBrace,
    EndOfFile,
    EndOfLine,
    Error
};

class Lexeme {
public:
    Lexeme(std::string s, LexemeType type, int id) : name_(std::move(s)), type_(type), code_id_(id) {}
    std::string toString() {
        std::string s;
        switch (this->type_) {
            case LexemeType::Service:
                s = "Service";
                return s;
            case LexemeType::Identifier:
                s = "Identifier";
                return s;
            case LexemeType::Literal:
                s = "Literal";
                return s;
            case LexemeType::Operator:
                s = "Operator";
                return s;
            case LexemeType::Comma:
                s = "Comma";
                return s;
            case LexemeType::Dot:
                s = "Dot";
                return s;
            case LexemeType::Brace:
                s = "Brace";
                return s;
            case LexemeType::SquareBrace:
                s = "Square brace";
                return s;
            case LexemeType::OpenBrace:
                s = "Open brace";
                return s;
            case LexemeType::CloseBrace:
                s = "Close brace";
                return s;
            case LexemeType::EndOfFile:
                s = "End of file";
                return s;
            case LexemeType::EndOfLine:
                s = "End of line";
                return s;
            case LexemeType::Error:
                s = "Error";
                return s;
        }
    }
    std::string getName() const {return name_;}

    LexemeType getType() const {return type_;}

    int getId() const {return code_id_;}
private:
    LexemeType type_;
    int code_id_;
    std::string name_;
};
