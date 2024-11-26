//
// Created by Данил Подлягин on 22.10.2024.
//

#include <iostream>
#include "Translator.h"
#include "LexicalAnalyzer.h"

Translator::Translator() : sem_stack_(varChecker_) {
    std::ifstream in("../src/builtInFunctions");
    std::string id, type;
    while (in >> id >> type) funcChecker_.pushId(type, id);
}

void Translator::run() {
//    Lexeme lexeme = lex_analyzer_.getLexeme();
//    while (lexeme.getType() != LexemeType::EndOfFile) {
//        std::cout << "\e[1;32mLexeme: \e[1;33m" << lexeme.getName() << "\e[0m    ";
//        std::cout << "\e[1;32mType: \e[0m" << lexeme.toString() << "    ";
//        std::cout << "\e[1;32mLine: \e[0m" << lexeme.getId() << '\n';
//        std::cout << "-------------------------------------\n";
//        lexeme = lex_analyzer_.getLexeme();
//    }
    startScanning();
}

bool Translator::isType(Lexeme lexeme) {
    return (lexeme.getName() == "integer" || lexeme.getName() == "float" || lexeme.getName() == "bool" ||
            lexeme.getName() == "char" || lexeme.getName() == "void" || lexeme.getName() == "string") &&
           lexeme.getType() == LexemeType::Service;
}

bool Translator::isLiteral(Lexeme lexeme) {
    return lexeme.getType() == LexemeType::FloatLiteral || lexeme.getType() == LexemeType::IntegerLiteral ||
           lexeme.getType() == LexemeType::BoolLiteral || lexeme.getType() == LexemeType::StringLiteral;
}

void Translator::startScanning() {
    try {
        startState();
        std::cout << "OK" << '\n';
    } catch (Lexeme lex) {
        if (lex.getType() == LexemeType::Error) {
            std::cerr << "Unknown lexeme: " + lex.getName() << '\n';
            std::cerr << lex.getId() << " string\n";
        } else {
            std::cerr << "Lexeme in wrong place: " + lex.getName() << '\n';
            std::cerr << "Line #" << lex.getId() << "\n";
        }
    } catch (Error error) {
        std::cerr << error.what() << '\n';
    }
    Lexeme lexeme = lex_analyzer_.getLexeme();
    std::cout << "Last lexeme: " << lexeme.getName() << "\n";
    std::cout << "Line #" << lexeme.getId() << "\n";
}

void Translator::startState() {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    while (lexeme.getType() != LexemeType::EndOfFile) {
        if (lexeme.getType() == LexemeType::Error) throw lexeme;
        else if (lexeme.getType() == LexemeType::Service) {
            if (lexeme.getName() == "Main" && lexeme.getType() == LexemeType::Service) mainState();
            else if (isType(lexeme)) definitionState(lexeme.getName());
            else if (lexeme.getName() == "const" && lexeme.getType() == LexemeType::Service) constDefinitionState();
            else if (lexeme.getType() == LexemeType::Service && lexeme.getName() == "array") arrayInitializationState();
            else throw lexeme;
        } else throw lexeme;
        lexeme = lex_analyzer_.getLexeme();
    }
}

void Translator::constDefinitionState() {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() != LexemeType::Service || !isType(lexeme)) throw lexeme;
    lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() != LexemeType::Identifier) throw lexeme;
    lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::EndOfLine) {
        startState();
        return;
    }
    if (lexeme.getType() == LexemeType::Operator && lexeme.getName() == "=") {
        calculationState();
    }
    lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::EndOfLine) {
        startState();
        return;
    }
    throw lexeme;
}

void Translator::definitionState(std::string type) {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::Operator && lexeme.getName() == "<-") functionDefinitionState(type);
    else if (lexeme.getType() == LexemeType::Identifier) variableInitializationState(type, lexeme.getName());
    else throw lexeme;
}

void Translator::functionDefinitionState(std::string type) {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() != LexemeType::Identifier) throw lexeme;
    varChecker_.createScope();
    std::string id = lexeme.getName();
    type += ":func:";
    lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() != LexemeType::OpenBrace) throw lexeme;
    lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() != LexemeType::CloseBrace) {
        lex_analyzer_.getBack(lexeme);
        do {
            lexeme = lex_analyzer_.getLexeme();
            if (!isType(lexeme)) throw lexeme;
            std::string varType = lexeme.getName();
            type += lexeme.getName() + ',';
            lexeme = lex_analyzer_.getLexeme();
            if (lexeme.getType() != LexemeType::Identifier) throw lexeme;
            varChecker_.pushId(varType, lexeme.getName());
            lexeme = lex_analyzer_.getLexeme();
            if (lexeme.getType() != LexemeType::CloseBrace && lexeme.getType() != LexemeType::Comma) throw lexeme;
        } while (lexeme.getType() != LexemeType::CloseBrace);
    }
    funcChecker_.pushId(type, id);
    std::string TypeStr;
    for (char c : type) {
        if (c == ':') break;
        TypeStr.push_back(c);
    }
    LexemeType Type;
    if (TypeStr == "integer") Type = LexemeType::IntegerLiteral;
    else if (TypeStr == "float") Type = LexemeType::FloatLiteral;
    else if (TypeStr == "string") Type = LexemeType::StringLiteral;
    else if (TypeStr == "bool") Type = LexemeType::BoolLiteral;
    lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::Brace && lexeme.getName() == "{") {
        lex_analyzer_.getBack(lexeme);
        blockState(Type, 0);
        varChecker_.exitScope();
        return;
    }
    lex_analyzer_.getBack(lexeme);
    operatorState(Type, 0);
    varChecker_.exitScope();
}

void Translator::variableInitializationState(std::string type, std::string id) {
    varChecker_.pushId(type, id);
    Lexeme lex;
    lex.setName(id);
    lex.setValue(true);
    if (type == "integer") lex.setType(LexemeType::IntegerLiteral);
    else if (type == "float") lex.setType(LexemeType::FloatLiteral);
    else if (type == "bool") lex.setType(LexemeType::BoolLiteral);
    else if (type == "string") lex.setType(LexemeType::StringLiteral);
    // void - ?
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::EndOfLine) {
        return;
    }
    if (lexeme.getType() == LexemeType::Operator && lexeme.getName() == "=") {
        sem_stack_.push(lex);
        sem_stack_.push(lexeme);
        calc8State();
        sem_stack_.checkBin();
        lexeme = lex_analyzer_.getLexeme();
    }
    if (lexeme.getType() == LexemeType::Comma) {
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::Identifier) throw lexeme;
        variableInitializationState(type, lexeme.getName());
        return;
    }
    if (lexeme.getType() == LexemeType::EndOfLine) {
        return;
    }
    throw lexeme;
}

void Translator::mainState() {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::Brace && lexeme.getName() == "{") {
        lex_analyzer_.getBack(lexeme);
        blockState(LexemeType::Error, 0);
        return;
    }
    lex_analyzer_.getBack(lexeme);
    operatorState(LexemeType::Error, 0);

}

void Translator::calculationState() {
    calc8State();
    Lexeme lexeme = lex_analyzer_.getLexeme();
    while (lexeme.getName() == "," && lexeme.getType() == LexemeType::Comma) {
        calc8State();
        lexeme = lex_analyzer_.getLexeme();
    }
    lex_analyzer_.getBack(lexeme);
}

void Translator::calc8State() {
    calc7State();
    Lexeme lexeme = lex_analyzer_.getLexeme();
    while (lexeme.getName() == "or" && lexeme.getType() == LexemeType::Operator) {
        sem_stack_.push(lexeme);
        calc7State();
        sem_stack_.checkBin();
        lexeme = lex_analyzer_.getLexeme();
    }
    lex_analyzer_.getBack(lexeme);
}

void Translator::calc7State() {
    calc6State();
    Lexeme lexeme = lex_analyzer_.getLexeme();
    while (lexeme.getName() == "and" && lexeme.getType() == LexemeType::Operator) {
        sem_stack_.push(lexeme);
        calc6State();
        sem_stack_.checkBin();
        lexeme = lex_analyzer_.getLexeme();
    }
    lex_analyzer_.getBack(lexeme);
}

void Translator::calc6State() {
    calc5State();
    Lexeme lexeme = lex_analyzer_.getLexeme();
    while (lexeme.getName() == "==" || lexeme.getName() == "!=" && lexeme.getType() == LexemeType::Operator) {
        sem_stack_.push(lexeme);
        calc5State();
        sem_stack_.checkBin();
        lexeme = lex_analyzer_.getLexeme();
    }
    lex_analyzer_.getBack(lexeme);
}

void Translator::calc5State() {
    calc4State();
    Lexeme lexeme = lex_analyzer_.getLexeme();
    while (lexeme.getType() == LexemeType::Operator &&
           lexeme.getName() == "<" || lexeme.getName() == "<=" ||
           lexeme.getName() == ">" || lexeme.getName() == ">=") {
        sem_stack_.push(lexeme);
        calc4State();
        sem_stack_.checkBin();
        lexeme = lex_analyzer_.getLexeme();
    }
    lex_analyzer_.getBack(lexeme);
}

void Translator::calc4State() {
    calc3State();
    Lexeme lexeme = lex_analyzer_.getLexeme();
    while (lexeme.getType() == LexemeType::Operator &&
           lexeme.getName() == "+" || lexeme.getName() == "-") {
        sem_stack_.push(lexeme);
        calc3State();
        sem_stack_.checkBin();
        lexeme = lex_analyzer_.getLexeme();
    }
    lex_analyzer_.getBack(lexeme);
}

void Translator::calc3State() {
    calc2State();
    Lexeme lexeme = lex_analyzer_.getLexeme();
    while (lexeme.getType() == LexemeType::Operator &&
           lexeme.getName() == "*" || lexeme.getName() == "/" || lexeme.getName() == "%") {
        sem_stack_.push(lexeme);
        calc2State();
        sem_stack_.checkBin();
        lexeme = lex_analyzer_.getLexeme();
    }
    lex_analyzer_.getBack(lexeme);
}

void Translator::calc2State() {
    calc1State();
    Lexeme lexeme = lex_analyzer_.getLexeme();
    while (lexeme.getType() == LexemeType::Operator &&
           lexeme.getName() == "++" || lexeme.getName() == "--") {
        sem_stack_.push(lexeme);
        calc1State();
        sem_stack_.checkBin();
        lexeme = lex_analyzer_.getLexeme();
    }
    lex_analyzer_.getBack(lexeme);
}

void Translator::calc1State() {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::Operator && lexeme.getName() == "-") {
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() == LexemeType::Identifier) {
            lex_analyzer_.getBack(lexeme);
            calc1State();
            return;
        }
        if (isLiteral(lexeme)) {
            lex_analyzer_.getBack(lexeme);
            calc1State();
            return;
        }
    }
    if (lexeme.getType() == LexemeType::Operator && lexeme.getName() == "!") {
        sem_stack_.push(lexeme);
        calc1State();
        sem_stack_.checkUno();
    }
    if (lexeme.getType() == LexemeType::OpenBrace) {
        calculationState();
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::CloseBrace) throw lexeme;
        return;
    }
    if (isLiteral(lexeme)) {
        sem_stack_.push(lexeme);
        return;
    }
    if (lexeme.getType() == LexemeType::Identifier) {
        Lexeme pref_lex = lexeme;
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() == LexemeType::OpenBrace) {
            if (!funcChecker_.checkId(pref_lex.getName())) throw Error("Undefined function: " + pref_lex.getName());
            lex_analyzer_.getBack(lexeme);
            lex_analyzer_.getBack(pref_lex);
            functionCallState();
            return;
        }
        if (!varChecker_.checkId(pref_lex.getName())) throw Error("Undefined variable: " + pref_lex.getName());
//        if (lexeme.getType() == LexemeType::Dot) {
//            functionCallState();
//            return;
//        }
        if (lexeme.getType() == LexemeType::SquareBrace && lexeme.getName() == "[") {
            pref_lex.setValue(true);
            pref_lex.setType(LexemeType::Array);
            sem_stack_.push(pref_lex);
            sem_stack_.push(lexeme);
            calculationState();
            sem_stack_.checkBin();
            lexeme = lex_analyzer_.getLexeme();
            if (lexeme.getType() != LexemeType::SquareBrace || lexeme.getName() != "]") throw lexeme;
            return;
        }
        if (lexeme.getType() == LexemeType::Operator && lexeme.getName() == "=") {
            pref_lex.setValue(true);
            if (varChecker_.getType(pref_lex.getName()) == "integer") {
                pref_lex.setType(LexemeType::IntegerLiteral);
            } else if (varChecker_.getType(pref_lex.getName()) == "float") {
                pref_lex.setType(LexemeType::FloatLiteral);
            } else if (varChecker_.getType(pref_lex.getName()) == "bool") {
                pref_lex.setType(LexemeType::BoolLiteral);
            } else if (varChecker_.getType(pref_lex.getName()) == "string") {
                pref_lex.setType(LexemeType::StringLiteral);
            }
            sem_stack_.push(pref_lex);
            sem_stack_.push(lexeme);
            calculationState();
            sem_stack_.checkBin();
            return;
        }
        pref_lex.setValue(true);
        if (varChecker_.getType(pref_lex.getName()) == "integer") {
            pref_lex.setType(LexemeType::IntegerLiteral);
        } else if (varChecker_.getType(pref_lex.getName()) == "float") {
            pref_lex.setType(LexemeType::FloatLiteral);
        } else if (varChecker_.getType(pref_lex.getName()) == "bool") {
            pref_lex.setType(LexemeType::BoolLiteral);
        } else if (varChecker_.getType(pref_lex.getName()) == "string") {
            pref_lex.setType(LexemeType::StringLiteral);
        }
        sem_stack_.push(pref_lex);
        lex_analyzer_.getBack(lexeme);
        return;
    }
    if (!(lexeme.getType() == LexemeType::Operator && lexeme.getName() == "--") &&
        !(lexeme.getType() == LexemeType::Operator && lexeme.getName() == "++"))
        throw lexeme;
    else lex_analyzer_.getBack(lexeme);
}

void Translator::functionCallState() {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() != LexemeType::Identifier) throw lexeme;
    std::string s = funcChecker_.getType(lexeme.getName());
    std::string type;
    for (char c : s) {
        if (c == ':') break;
        type.push_back(c);
    }
    Lexeme res;
    if (type == "integer") {
        res.setType(LexemeType::IntegerLiteral);
    }
    if (type == "string") {
        res.setType(LexemeType::StringLiteral);
    }
    if (type == "char") {
        res.setType(LexemeType::IntegerLiteral);
    }
    if (type == "bool") {
        res.setType(LexemeType::BoolLiteral);
    }
    if (type == "float") {
        res.setType(LexemeType::FloatLiteral);
    }
    res.setValue(false);
    sem_stack_.push(res);
    if (!funcChecker_.checkId(lexeme.getName())) throw lexeme;
    lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() != LexemeType::OpenBrace) throw lexeme;
    lexeme = lex_analyzer_.getLexeme();
    type += ":func:";
    while (lexeme.getType() != LexemeType::CloseBrace) {
        lex_analyzer_.getBack(lexeme);
        calc8State();
        Lexeme resLexeme = sem_stack_.pop();
        type += resLexeme.toString() + ',';
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::Comma && lexeme.getType() != LexemeType::CloseBrace) throw lexeme;
        if (lexeme.getType() != LexemeType::CloseBrace) lexeme = lex_analyzer_.getLexeme();
    }
    if (s != type) throw Error("Undefined function: " + type);
}

void Translator::blockState(LexemeType isFunc, bool isLoop) {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    varChecker_.createScope();
    if (lexeme.getType() != LexemeType::Brace || lexeme.getName() != "{") throw lexeme;
    while (lexeme.getType() != LexemeType::Brace || lexeme.getName() != "}") {
        operatorState(isFunc, isLoop);
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::Brace || lexeme.getName() != "}") lex_analyzer_.getBack(lexeme);
    }
    varChecker_.exitScope();
}

void Translator::operatorState(LexemeType isFunc, bool isLoop) {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (isType(lexeme)) { // initialization
        definitionState(lexeme.getName());
        return;
    }
    if (lexeme.getType() == LexemeType::Service && lexeme.getName() == "array") {
        arrayInitializationState();
        return;
    }
    if (lexeme.getType() == LexemeType::Service && lexeme.getName() == "const") { // const initialization
        constDefinitionState();
        return;
    }
    if (lexeme.getName() == "if" && lexeme.getType() == LexemeType::Service) { // condition
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::OpenBrace) throw lexeme;
        calculationState();
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::CloseBrace) throw lexeme;
        Lexeme lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() == LexemeType::Brace && lexeme.getName() == "{") {
            lex_analyzer_.getBack(lexeme);
            blockState(isFunc, isLoop);
            return;
        }
        lex_analyzer_.getBack(lexeme);
        operatorState(isFunc, isLoop);
        return;
    }
    if (lexeme.getName() == "for" && lexeme.getType() == LexemeType::Service) {
        varChecker_.createScope();
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::OpenBrace) throw lexeme;
        lexeme = lex_analyzer_.getLexeme();
        if (isType(lexeme)) {
            definitionState(lexeme.getName());
        } else {
            lex_analyzer_.getBack(lexeme);
            calculationState();
            lexeme = lex_analyzer_.getLexeme();
            if (lexeme.getType() != LexemeType::EndOfLine) throw lexeme;
        }
        calculationState();
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::EndOfLine) throw lexeme;
        calculationState();
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::CloseBrace) throw lexeme;

        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() == LexemeType::Brace && lexeme.getName() == "{") {
            lex_analyzer_.getBack(lexeme);
            blockState(isFunc, isLoop);
            varChecker_.exitScope();
            return;
        }
        lex_analyzer_.getBack(lexeme);
        operatorState(isFunc, isLoop);
        varChecker_.exitScope();
        return;
    }
    if (lexeme.getType() == LexemeType::Service && lexeme.getName() == "while") {
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::OpenBrace) throw lexeme;
        calculationState();
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::CloseBrace) throw lexeme;
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() == LexemeType::Brace && lexeme.getName() == "{") {
            lex_analyzer_.getBack(lexeme);
            blockState(isFunc, true);
            return;
        }
        lex_analyzer_.getBack(lexeme);
        operatorState(isFunc, true);
        return;
    }
    if (lexeme.getType() == LexemeType::Service && lexeme.getName() == "break") {
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::EndOfLine) throw lexeme;
        if (!isLoop) throw Error("Can`t break not in loop");
        return;
    }
    if (lexeme.getType() == LexemeType::Service && lexeme.getName() == "continue") {
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::EndOfLine) throw lexeme;
        if (!isLoop) throw Error("Can`t continue not in loop");
        return;
    }
    if (lexeme.getType() == LexemeType::Service && lexeme.getName() == "return") {
        Lexeme lex;
        lex.setType(isFunc);
        lex.setValue(false);
        sem_stack_.push(lex);
        sem_stack_.push(lexeme);
        calculationState();
        sem_stack_.checkBin();
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::EndOfLine) throw lexeme;
        return;
    }
    if (lexeme.getName() == "switch" && lexeme.getType() == LexemeType::Service) {
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::OpenBrace) throw lexeme;
        calculationState();
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::CloseBrace) throw lexeme;
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::Brace && lexeme.getName() != "{") throw lexeme;
        switchState(isFunc, isLoop);
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::Brace && lexeme.getName() != "}") throw lexeme;
        return;
    }
    lex_analyzer_.getBack(lexeme);
    calculationState();
    lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() != LexemeType::EndOfLine) throw lexeme;
}

void Translator::switchState(LexemeType isFunc, bool isLoop) {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    while (lexeme.getType() == LexemeType::Service && lexeme.getName() == "case") {
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::OpenBrace) throw lexeme;
        calculationState();
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::CloseBrace) throw lexeme;
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() == LexemeType::Brace && lexeme.getName() == "{") {
            lex_analyzer_.getBack(lexeme);
            blockState(isFunc, isLoop);
        } else {
            lex_analyzer_.getBack(lexeme);
            operatorState(isFunc, isLoop);
        }
        lexeme = lex_analyzer_.getLexeme();
    }
    if (lexeme.getType() == LexemeType::Service && lexeme.getName() == "default") {
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() == LexemeType::Brace && lexeme.getName() == "{") {
            blockState(isFunc, isLoop);
            lexeme = lex_analyzer_.getLexeme();
            if (lexeme.getType() != LexemeType::Brace && lexeme.getName() != "}") throw lexeme;
            return;
        }
        lex_analyzer_.getBack(lexeme);
        operatorState(isFunc, isLoop);
        return;
    }
    if (lexeme.getType() == LexemeType::Brace && lexeme.getName() == "}") {
        lex_analyzer_.getBack(lexeme);
        return;
    }
    throw lexeme;
}

void Translator::arrayInitializationState() {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() != LexemeType::Operator || lexeme.getName() != "<-") throw lexeme;
    lexeme = lex_analyzer_.getLexeme();
    if (!isType(lexeme)) throw lexeme;
    std::string type = "array:" + lexeme.getName();
    lexeme = lex_analyzer_.getLexeme();
    while (true) {
        if (lexeme.getType() != LexemeType::Identifier) throw lexeme;
        varChecker_.pushId(type, lexeme.getName());
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() == LexemeType::Comma) lexeme = lex_analyzer_.getLexeme();
        else if (lexeme.getType() == LexemeType::EndOfLine) break;
        else throw lexeme;
    }
}