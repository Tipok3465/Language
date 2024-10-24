#include "LexicalAnalyzer.h"

LexicalAnalyzer::LexicalAnalyzer() : cur_(0), id_(0), line_(1) {
    std::ifstream f("../src/code.txt");
    f.seekg(0, std::ios::end);
    size_ = f.tellg();
    f.seekg(0);
    code_text_ = new char[size_];
    f.read(code_text_, static_cast<std::streamsize>(size_));
    std::cout << code_text_ << "\n-------------------------------------\n";
    f.close();
    createServiceBor();
    createOperatorBor();
}

LexicalAnalyzer::~LexicalAnalyzer() {
    delete[] code_text_;
}

void LexicalAnalyzer::createServiceBor() {
    std::ifstream f("../src/lexemes.txt");
    while (f) {
        std::string lex;
        f >> lex;
        serviceBor_.add(lex);
    }
    f.close();
}

void LexicalAnalyzer::createOperatorBor() {
    std::ifstream f("../src/operator.txt");
    while (f) {
        std::string lex;
        f >> lex;
        operatorBor_.add(lex);
    }
    f.close();
}

Lexeme LexicalAnalyzer::getLexeme() {
    char c = getSymbol();
    while (c == EOF || c == ' ' || c == '\n') {
        if (c == EOF) return {"End of file", LexemeType::EndOfFile, line_};
        if (c == '\n') ++line_;
        c = getSymbol();
    }
    if (c == '"') {
        std::string s;
        c = getSymbol();
        while (c != '"') {
            if (c == EOF) throw "Not ended literal";
            s.push_back(c);
            c = getSymbol();
        }
        return {s, LexemeType::Literal, line_};
    }
    if (contains('0', '9', c)) {
        std::string s;
        while (contains('0', '9', c)) {
            s.push_back(c);
            c = getSymbol();
        }
        --cur_;
        return {s, LexemeType::Literal, line_};
    }
    if (contains('a', 'z', c) || contains('A', 'Z', c) || contains('_', '_', c)) {
        std::string s;
        while (contains('0', '9', c) || contains('a', 'z', c) ||
               contains('A', 'Z', c) || contains('_', '_', c)) {
            s.push_back(c);
            c = getSymbol();
        }
        if (c != EOF) --cur_;
        return {s,
                serviceBor_.check(s) ? LexemeType::Service : LexemeType::Identifier,
                line_};
    }
    if (operatorBor_.check(std::string{c})) {
        char nc = getSymbol();
        std::string s{c, nc};
        if (s == "//") {
            c = getSymbol();
            while (c != '\n') c = getSymbol();
            ++line_;
            return getLexeme();
        }
        if (operatorBor_.check(s)) {
            return {s, LexemeType::Operator, line_};
        }
        --cur_;
        s.pop_back();
        return {s, LexemeType::Operator, line_};
    }
    switch (c) {
        case '.': return {".", LexemeType::Dot, line_};
        case ',': return {",", LexemeType::Comma, line_};
        case '(': return {"(", LexemeType::OpenBrace, line_};
        case ')': return {")", LexemeType::CloseBrace, line_};
        case ';': return {";", LexemeType::EndOfLine, line_};
        case '[': return {"[", LexemeType::SquareBrace, line_};
        case ']': return {"]", LexemeType::SquareBrace, line_};
        case '{': return {"{", LexemeType::Brace, line_};
        case '}': return {"}", LexemeType::Brace, line_};
    }
    return {"Error", LexemeType::Error, line_};
}

char LexicalAnalyzer::getSymbol() {
    if (cur_ >= size_) return EOF;
    return code_text_[cur_++];
}

bool LexicalAnalyzer::contains(int leftBorder, int rightBorder, int val) {
    return leftBorder <= val && val <= rightBorder;
}
