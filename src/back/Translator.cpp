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
        Lexeme lex;
        lex.setType(LexemeType::EndOfFile);
        lex.setName("EndOfFile");
        generator_.pushPoliz(lex);
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
    std::cout << "Line #";
    std::cout << lexeme.getId() << "\n";
    std::cout << "POLIZ:\n";
    std::vector<Lexeme> poliz = generator_.getPoliz();
    for (int i = 0; i < (int)poliz.size(); ++i) {
        std::cout << "\x1B[37m";
        std::cout << i << ' ';
        std::cout << "\x1B[0m";
        std::cout << "\x1B[29m";
        std::cout << poliz[i].getName() << ' ';
        std::cout << "\033[0m";
        std::cout << "\x1B[35m";
        std::cout << poliz[i].toString() << ' ';
        std::cout << "\033[0m";
        std::cout << "ID: ";
        std::cout << "\x1B[37m";
        std::cout << poliz[i].getId() << ' ';
        std::cout << "\x1B[0m";
        if (poliz[i].getType() == LexemeType::PolizField) {
            std::cout << "field id: " << poliz[i].getFieldId();
        }
        std::cout << '\n';
    }
    printf("\x1B[37mTexting\033[0m\t\t");
    printf("\x1B[93mTexting\033[0m\n");

    printf("\033[3;42;30mTexting\033[0m\t\t");
    printf("\033[3;43;30mTexting\033[0m\t\t");
    printf("\033[3;44;30mTexting\033[0m\t\t");
    printf("\033[3;104;30mTexting\033[0m\t\t");
    printf("\033[3;100;30mTexting\033[0m\n");

    printf("\033[3;47;35mTexting\033[0m\t\t");
    printf("\033[2;47;35mTexting\033[0m\t\t");
    printf("\033[1;47;35mTexting\033[0m\t\t");
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
    generator_.pushPoliz(lexeme);
    lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::EndOfLine) {
        generator_.pushStack(lexeme);
        startState();
        return;
    }
    if (lexeme.getType() == LexemeType::Operator && lexeme.getName() == "=") {
        generator_.pushStack(lexeme);
        calculationState();
    }
    lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::EndOfLine) {
        generator_.pushStack(lexeme);
        startState();
        return;
    }
    throw lexeme;
}

void Translator::definitionState(std::string type) {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::Operator && lexeme.getName() == "<-") functionDefinitionState(type);
    else if (lexeme.getType() == LexemeType::Identifier){
        generator_.pushPoliz(lexeme);
        variableInitializationState(type, lexeme.getName());
    }
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
        generator_.pushStack(lexeme);
        return;
    }
    if (lexeme.getType() == LexemeType::Operator && lexeme.getName() == "=") {
        sem_stack_.push(lex);
        sem_stack_.push(lexeme);
        generator_.pushStack(lexeme);
        calc8State();
        sem_stack_.checkBin();
        lexeme = lex_analyzer_.getLexeme();
    }
    if (lexeme.getType() == LexemeType::Comma) {
        lexeme = lex_analyzer_.getLexeme();
        generator_.pushStack(lexeme);
        if (lexeme.getType() != LexemeType::Identifier) throw lexeme;
        variableInitializationState(type, lexeme.getName());
        return;
    }
    if (lexeme.getType() == LexemeType::EndOfLine) {
        generator_.pushStack(lexeme);
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
        generator_.pushStack(lexeme);
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
        generator_.pushStack(lexeme);
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
        generator_.pushStack(lexeme);
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
        generator_.pushStack(lexeme);
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
        generator_.pushStack(lexeme);
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
        generator_.pushStack(lexeme);
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
        generator_.pushStack(lexeme);
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
        generator_.pushStack(lexeme);
        calc1State();
        sem_stack_.checkBin();
        lexeme = lex_analyzer_.getLexeme();
    }
    lex_analyzer_.getBack(lexeme);
}

void Translator::calc1State() {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() == LexemeType::Operator && lexeme.getName() == "-") {
        lexeme.setType(LexemeType::UnaryMinus);
        generator_.pushStack(lexeme);
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
        generator_.pushStack(lexeme);
        sem_stack_.push(lexeme);
        calc1State();
        sem_stack_.checkUno();
    }
    if (lexeme.getType() == LexemeType::OpenBrace) {
        generator_.pushStack(lexeme);
        calculationState();
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::CloseBrace) throw lexeme;
        generator_.pushStack(lexeme);
        return;
    }
    if (isLiteral(lexeme)) {
        sem_stack_.push(lexeme);
        generator_.pushPoliz(lexeme);
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
        if (lexeme.getType() == LexemeType::SquareBrace && lexeme.getName() == "[") {
            Lexeme lex1;
            lex1.setType(LexemeType::OpenBrace);
            lex1.setName("(");
            generator_.pushStack(lex1);
            pref_lex.setValue(true);
            pref_lex.setType(LexemeType::Array);
            sem_stack_.push(pref_lex);
            sem_stack_.push(lexeme);
            generator_.pushPoliz(pref_lex);
            calculationState();
            lex1.setType(LexemeType::CloseBrace);
            lex1.setName(")");
            generator_.pushStack(lex1);
            generator_.pushPoliz(lexeme);
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
            generator_.pushPoliz(pref_lex);
            generator_.pushStack(lexeme);
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
        generator_.pushPoliz(pref_lex);
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
    Lexeme func = lexeme;
    func.setType(LexemeType::FuncCall);
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
        generator_.clearStack();
        Lexeme resLexeme = sem_stack_.pop();
        type += resLexeme.toString() + ',';
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::Comma && lexeme.getType() != LexemeType::CloseBrace) throw lexeme;
        if (lexeme.getType() != LexemeType::CloseBrace) lexeme = lex_analyzer_.getLexeme();
    }
    if (s != type) throw Error("Undefined function: " + type);
    generator_.pushPoliz(func);
}

void Translator::blockState(LexemeType isFunc, bool isLoop, bool isNewScope) {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    if (isNewScope) varChecker_.createScope();
    if (lexeme.getType() != LexemeType::Brace || lexeme.getName() != "{") throw lexeme;
    while (lexeme.getType() != LexemeType::Brace || lexeme.getName() != "}") {
        operatorState(isFunc, isLoop);
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::Brace || lexeme.getName() != "}") lex_analyzer_.getBack(lexeme);
    }
    if (isNewScope) varChecker_.exitScope();
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
        Lexeme lex1;
        lex1.setType(LexemeType::OpenBrace);
        lex1.setName("(");
        generator_.pushStack(lex1);
        calculationState();
        lex1.setType(LexemeType::CloseBrace);
        lex1.setName(")");
        generator_.pushStack(lex1);
        Lexeme end_lex;
        int pointer_id = generator_.getPos();
        Lexeme lex;
        lex.setType(LexemeType::PolizField);
        lex.setName("PolizField");
        generator_.pushPoliz(lex);
        lex1.setType(LexemeType::GoToFalse);
        lex1.setName("GoToFalse");
        generator_.pushPoliz(lex1);
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::CloseBrace) throw lexeme;
        Lexeme lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() == LexemeType::Brace && lexeme.getName() == "{") {
            lex_analyzer_.getBack(lexeme);
            blockState(isFunc, isLoop);
            int pos = generator_.getPos();
            generator_.setPos(pointer_id, pos);
            return;
        }

        lex_analyzer_.getBack(lexeme);
        operatorState(isFunc, isLoop);
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() == LexemeType::Service && lexeme.getName() == "else") {
            lexeme = lex_analyzer_.getLexeme();
            if (lexeme.getType() == LexemeType::Brace && lexeme.getName() == "{") {
                lex_analyzer_.getBack(lexeme);
                blockState(isFunc, isLoop);
                int pos = generator_.getPos();
                generator_.setPos(pointer_id, pos);
                return;
            }
            lex_analyzer_.getBack(lexeme);
            operatorState(isFunc, isLoop);
        } else
            lex_analyzer_.getBack(lexeme);
        int pos = generator_.getPos();
        generator_.setPos(pointer_id, pos);
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
            generator_.pushStack(lexeme);
        }
        int cond_pos = generator_.getPos();
        Lexeme lex1;
        lex1.setType(LexemeType::OpenBrace);
        lex1.setName("(");
        generator_.pushStack(lex1);
        calculationState();
        lex1.setType(LexemeType::CloseBrace);
        lex1.setName(")");
        generator_.pushStack(lex1);

        lex1.setType(LexemeType::PolizField);
        lex1.setName("PolizField");
        int pos1 = generator_.getPos();
        generator_.pushPoliz(lex1);
        lex1.setType(LexemeType::GoToFalse);
        lex1.setName("GoToFalse");
        generator_.pushPoliz(lex1);

        lex1.setType(LexemeType::PolizField);
        lex1.setName("PolizField");
        int pos2 = generator_.getPos();
        generator_.pushPoliz(lex1);
        lex1.setType(LexemeType::GoToAnyway);
        lex1.setName("GoToAnyway");
        generator_.pushPoliz(lex1);

        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::EndOfLine) throw lexeme;
        generator_.pushStack(lexeme);
        int step_pos = generator_.getPos();
        Lexeme lex2;
        lex2.setType(LexemeType::OpenBrace);
        lex2.setName("(");
        generator_.pushStack(lex2);
        calculationState();
        lex2.setType(LexemeType::CloseBrace);
        lex2.setName(")");
        generator_.pushStack(lex2);
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::CloseBrace) throw lexeme;
        int pos3 = generator_.getPos();
        lex1.setType(LexemeType::PolizField);
        lex1.setName("PolizField");
        generator_.pushPoliz(lex1);
        generator_.setPos(pos3, cond_pos);
        lex1.setType(LexemeType::GoToAnyway);
        lex1.setName("GoToAnyway");
        generator_.pushPoliz(lex1);
        int exp_pos = generator_.getPos();
        generator_.setPos(pos2, exp_pos);
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() == LexemeType::Brace && lexeme.getName() == "{") {
            lex_analyzer_.getBack(lexeme);
            blockState(isFunc, true, false);
            varChecker_.exitScope();
            int exp_end_pos = generator_.getPos();
            lex1.setType(LexemeType::PolizField);
            lex1.setName("PolizField");
            generator_.pushPoliz(lex1);
            generator_.setPos(exp_end_pos, step_pos);
            lex1.setType(LexemeType::GoToAnyway);
            lex1.setName("GoToAnyway");
            generator_.pushPoliz(lex1);
            int end_pos = generator_.getPos();
            generator_.setPos(pos1, end_pos);
            return;
        }
        lex_analyzer_.getBack(lexeme);
        operatorState(isFunc, isLoop);
        varChecker_.exitScope();
        int exp_end_pos = generator_.getPos();
        lex1.setType(LexemeType::PolizField);
        lex1.setName("PolizField");
        generator_.pushPoliz(lex1);
        generator_.setPos(exp_end_pos, step_pos);
        lex1.setType(LexemeType::GoToAnyway);
        lex1.setName("GoToAnyway");
        generator_.pushPoliz(lex1);
        int end_pos = generator_.getPos();
        generator_.setPos(pos1, end_pos);
        return;
    }
    if (lexeme.getType() == LexemeType::Service && lexeme.getName() == "while") {
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::OpenBrace) throw lexeme;
        Lexeme lex1;
        lex1.setType(LexemeType::OpenBrace);
        lex1.setName("(");
        generator_.pushStack(lex1);
        int pos1 = generator_.getPos();
        calculationState();
        lex1.setType(LexemeType::CloseBrace);
        lex1.setName(")");
        generator_.pushStack(lex1);

        int pf = generator_.getPos();
        lex1.setType(LexemeType::PolizField);
        lex1.setName("PolizField");
        generator_.pushPoliz(lex1);
        lex1.setType(LexemeType::GoToFalse);
        lex1.setName("GoToFalse");
        generator_.pushPoliz(lex1);

        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::CloseBrace) throw lexeme;
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() == LexemeType::Brace && lexeme.getName() == "{") {
            lex_analyzer_.getBack(lexeme);
            blockState(isFunc, true);

            int p = generator_.getPos();
            lex1.setType(LexemeType::PolizField);
            lex1.setName("PolizField");
            generator_.pushPoliz(lex1);
            lex1.setType(LexemeType::GoToAnyway);
            lex1.setName("GoToAnyway");
            generator_.pushPoliz(lex1);
            generator_.setPos(p, pos1);

            int p_end = generator_.getPos();
            generator_.setPos(pf, p_end);
            return;
        }
        lex_analyzer_.getBack(lexeme);
        operatorState(isFunc, true);
        int p = generator_.getPos();
        lex1.setType(LexemeType::PolizField);
        lex1.setName("PolizField");
        generator_.pushPoliz(lex1);
        lex1.setType(LexemeType::GoToAnyway);
        lex1.setName("GoToAnyway");
        generator_.pushPoliz(lex1);
        generator_.setPos(p, pos1);

        int p_end = generator_.getPos();
        generator_.setPos(pf, p_end);
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
        Lexeme lex1;
        lex1.setType(LexemeType::Return);
        lex1.setName("Return");
        generator_.pushPoliz(lex1);
        sem_stack_.checkBin();
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::EndOfLine) throw lexeme;
        return;
    }
    if (lexeme.getName() == "switch" && lexeme.getType() == LexemeType::Service) {
        Lexeme switchLexeme = lexeme;
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::OpenBrace) throw lexeme;
        Lexeme lex1;
        lex1.setType(LexemeType::OpenBrace);
        lex1.setName("(");
        generator_.pushStack(lex1);
        calculationState();
        lex1.setType(LexemeType::CloseBrace);
        lex1.setName(")");
        generator_.pushStack(lex1);
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::CloseBrace) throw lexeme;
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::Brace && lexeme.getName() != "{") throw lexeme;
        sem_stack_.push(switchLexeme);
        switchState(isFunc, isLoop);
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::Brace && lexeme.getName() != "}") throw lexeme;
        return;
    }
    lex_analyzer_.getBack(lexeme);
    calculationState();
    lexeme = lex_analyzer_.getLexeme();
    if (lexeme.getType() != LexemeType::EndOfLine) throw lexeme;
    generator_.pushStack(lexeme);
}

void Translator::switchState(LexemeType isFunc, bool isLoop) {
    Lexeme lexeme = lex_analyzer_.getLexeme();
    std::vector<int> pos;
    int pref = -1;
    int cur;
    while (lexeme.getType() == LexemeType::Service && lexeme.getName() == "case") {
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() != LexemeType::OpenBrace) throw lexeme;
        Lexeme lex1;
        cur = generator_.getPos();
        lex1.setType(LexemeType::OpenBrace);
        lex1.setName("(");
        generator_.pushStack(lex1);
        calculationState();
        lex1.setType(LexemeType::CloseBrace);
        lex1.setName(")");
        generator_.pushStack(lex1);
        sem_stack_.checkBin();
        lex1.setType(LexemeType::SafeComparison);
        lex1.setName("==");
        generator_.pushPoliz(lex1);

        lex1.setType(LexemeType::PolizField); // false case -> go to next case
        lex1.setName("PolizField");
        int p = generator_.getPos();
        generator_.pushPoliz(lex1);
        if (pref != -1) {
            generator_.setPos(pref, cur);
        }
        pref = p;
        lex1.setType(LexemeType::GoToFalse);
        lex1.setName("GoToFalse");
        generator_.pushPoliz(lex1);

        lex1.setType(LexemeType::PolizField); // true case -> go to end of switch
        lex1.setName("PolizField");
        pos.push_back(generator_.getPos());
        generator_.pushPoliz(lex1);

        lex1.setType(LexemeType::GoToAnyway);
        lex1.setName("GoToAnyway");
        generator_.pushPoliz(lex1);

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
        int p = generator_.getPos();
        if (pref != -1) {
            generator_.setPos(pref, p);
        }
        if (lexeme.getType() == LexemeType::Brace && lexeme.getName() == "{") {
            blockState(isFunc, isLoop);
            lexeme = lex_analyzer_.getLexeme();
            if (lexeme.getType() != LexemeType::Brace && lexeme.getName() != "}") throw lexeme;
            int end_pos = generator_.getPos();
            for (auto e : pos) {
                generator_.setPos(e, end_pos);
            }
            return;
        }
        lex_analyzer_.getBack(lexeme);
        operatorState(isFunc, isLoop);
        int end_pos = generator_.getPos();
        for (auto e : pos) {
            generator_.setPos(e, end_pos);
        }
        return;
    }
    if (lexeme.getType() == LexemeType::Brace && lexeme.getName() == "}") {
        lex_analyzer_.getBack(lexeme);
        int end_pos = generator_.getPos();
        for (auto e : pos) {
            generator_.setPos(e, end_pos);
        }
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
        generator_.pushPoliz(lexeme);
        lexeme = lex_analyzer_.getLexeme();
        if (lexeme.getType() == LexemeType::Comma) {
            generator_.pushStack(lexeme);
            lexeme = lex_analyzer_.getLexeme();
        }
        else if (lexeme.getType() == LexemeType::EndOfLine) {
            generator_.pushStack(lexeme);
            break;
        }
        else throw lexeme;
    }
}