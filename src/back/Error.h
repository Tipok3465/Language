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