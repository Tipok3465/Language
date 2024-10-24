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
        if (c == EOF) return {"End of file", LexemeType::EndOfFile, id_};
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
        --cur_;
        return {s, LexemeType::Literal, id_};
    }
    if (contains('0', '9', c)) {
        std::string s;
        while (contains('0', '9', c)) {
            s.push_back(c);
            c = getSymbol();
        }
        --cur_;
        return {s, LexemeType::Literal, id_};
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
                id_};
    }
    if (operatorBor_.check(std::string{c})) {
        char nc = getSymbol();
        std::string s{c, nc};
        if (operatorBor_.check(s)) return {s, LexemeType::Operator, id_};
        --cur_;
        s.pop_back();
        return {s, LexemeType::Operator, id_};
    }
    switch (c) {
        case '.': return {".", LexemeType::Dot, id_};
        case ',': return {",", LexemeType::Comma, id_};
        case '(': return {"(", LexemeType::OpenBrace, id_};
        case ')': return {")", LexemeType::CloseBrace, id_};
        case ';': return {";", LexemeType::EndOfLine, id_};
        case '[': return {"[", LexemeType::SquareBrace, id_};
        case ']': return {"]", LexemeType::SquareBrace, id_};
        case '{': return {"{", LexemeType::Brace, id_};
        case '}': return {"}", LexemeType::Brace, id_};
    }
    return {"Error", LexemeType::Error, id_};
}

char LexicalAnalyzer::getSymbol() {
    if (cur_ >= size_) return EOF;
    return code_text_[cur_++];
}

bool LexicalAnalyzer::contains(int leftBorder, int rightBorder, int val) {
    return leftBorder <= val && val <= rightBorder;
}
