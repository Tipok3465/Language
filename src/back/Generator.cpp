#include "Generator.h"

int getPriority(Lexeme lex) {
    if (lex.getType() == LexemeType::OpenBrace) return 0;
    if (lex.getType() == LexemeType::CloseBrace) return 0;
    if (lex.getType() == LexemeType::Comma) return 0;
    if (lex.getType() == LexemeType::EndOfLine) return 0;
    std::string op = lex.getName();
    if (op == "=") return 0;
    if (op == "or") return 1;
    if (op == "and") return 2;
    if (op == ">" || op == ">=" || op == "<=" || op == "<") {
        return 3;
    }
    if (op == "==" || op == "!=") {
        return 4;
    }
    if (op == "+" || op == "-") {
        return 5;
    }
    if (op == "*" || op == "/" || op == "%") return 6;
    if (op == "!") return 7;
    if (lex.getType() == LexemeType::UnaryMinus) return 7;
    if (op == "++" || op == "--") return 8;
    if (lex.getType() == LexemeType::SquareBrace) return 9;
    if (lex.getType() == LexemeType::FuncCall) return 9;
    if (lex.getType() != LexemeType::Operator) {
        std::cout << "Fuck it\n";
        throw std::exception();
    }
    std::cout << "Unknown operator:(\n";
    throw std::exception();
}

void Generator::pushPoliz(Lexeme lex) {
    poliz_.push_back(lex);
    types_.push_back(lex.getType());
    ++cur_;
}

void Generator::pushStack(Lexeme lex) {
    int p = getPriority(lex);
    if (lex.getType() == LexemeType::OpenBrace) {
        stack_.push_back(lex);
        return;
    }
    if (lex.getType() == LexemeType::Comma) {
        while (!stack_.empty() && stack_.back().getType() != LexemeType::OpenBrace) {
            pushPoliz(stack_.back());
            stack_.pop_back();
        }
        return;
    }
    if (lex.getType() == LexemeType::EndOfLine) {
        clearStack();
        pushPoliz(lex);
        return;
    }
    if (lex.getType() == LexemeType::CloseBrace) {
        while (stack_.back().getType() != LexemeType::OpenBrace) {
            pushPoliz(stack_.back());
            stack_.pop_back();
        }
        stack_.pop_back();
        return;
    }
    while (!stack_.empty() && stack_.back().getPriority() >= p) {
        pushPoliz(stack_.back());
        stack_.pop_back();
    }
    lex.setPriority(p);
    stack_.push_back(lex);
}

void Generator::clearStack() {
    while (!stack_.empty()) {
        pushPoliz(stack_.back());
        stack_.pop_back();
    }
}

int Generator::getPos() {
    return cur_;
}

void Generator::setPos(int i, int x) {
    poliz_[i].setFieldId(x);
}

std::vector<Lexeme> Generator::getPoliz() {
    return poliz_;
}