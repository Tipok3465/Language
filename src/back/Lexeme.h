//
// Created by Данил Подлягин on 22.10.2024.
//
#pragma once
#include <utility>

#include "string"

enum class LexemeType {
    Service,
    Identifier,
    FloatLiteral,
    IntegerLiteral,
    StringLiteral,
    BoolLiteral,
    Array,
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
    Lexeme() = default;
    Lexeme(std::string s, LexemeType type, int id) : name_(std::move(s)), type_(type), code_id_(id) {}
    std::string toString() {
        std::string s;
        switch (this->type_) {
            case LexemeType::Service:
                s = "Service";
                return s;
            case LexemeType::Identifier:
                s = "identifier";
                return s;
            case LexemeType::FloatLiteral:
                s = "float";
                return s;
            case LexemeType::IntegerLiteral:
                s = "integer";
                return s;
            case LexemeType::StringLiteral:
                s = "string";
                return s;
            case LexemeType::BoolLiteral:
                s = "bool";
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
            case LexemeType::Array:
                s = "array";
                return s;
        }
    }
    std::string getName() const {return name_;}
    void setName(std::string s) {name_ = s;}
    LexemeType getType() const {return type_;}
    void setType(LexemeType type) {type_ = type;}
    int getId() const {return code_id_;}
    int setId(int id) {code_id_ = id;}
    void setValue(bool f) {lvalue_ = f;}
    bool isLvalue() {return lvalue_;}
private:
    LexemeType type_;
    int code_id_;
    std::string name_;
    bool lvalue_ = false;
};
