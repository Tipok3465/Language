//
// Created by Egor on 19.11.2024.
//

#ifndef LANGUAGE_ERROR_H
#define LANGUAGE_ERROR_H

#endif //LANGUAGE_ERROR_H

#pragma once

#include <exception>
#include <string>

class Error: public std::exception
{
public:
    Error(const std::string& message): message{message} {}
    const char* what() const noexcept override {
        return message.c_str();
    }
private:
    std::string message;
};
