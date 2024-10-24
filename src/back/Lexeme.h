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
    Error,
};

class Lexeme {
public:
    Lexeme(std::string s, LexemeType type, int id) : name_(std::move(s)), type_(type), code_id_(id) {}

    std::string getName() const {return name_;}

    LexemeType getType() const {return type_;}

    int getId() const {return code_id_;}
private:
    LexemeType type_;
    int code_id_;
    std::string name_;
};
