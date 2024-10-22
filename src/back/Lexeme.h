//
// Created by Данил Подлягин on 22.10.2024.
//
#pragma once
#include <utility>

#include "string"

class Lexeme {
public:
    Lexeme(std::string s, int type, int id) : name_(std::move(s)), type_(type), code_id_(id) {}

    std::string getName() const {return name_;}

    int getType() const {return type_;}

    int getId() const {return code_id_;}
private:
    int type_;
    int code_id_;
    std::string name_;
};
