#include "LexicalAnalyzer.h"

LexicalAnalyzer::LexicalAnalyzer() {
    std::ifstream f("../src/code.txt");
    f.seekg(0, std::ios::end);
    size_t size = f.tellg();
    f.seekg(0);
    code_text_ = new char[size];
    f.read(code_text_, static_cast<std::streamsize>(size));
    std::cout << code_text_ << "\n-------------------------------------\n";
    f.close();
    createBor();
}

void LexicalAnalyzer::createBor() {
    std::ifstream f("../src/lexemes.txt");
    while (f) {
        std::string lex;
        f >> lex;
        bor.add(lex);
    }
    f.close();
}