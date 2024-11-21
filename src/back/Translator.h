//
// Created by Данил Подлягин on 22.10.2024.
//
#pragma once


#include "LexicalAnalyzer.h"
#include "NameChecker.h"
#include "iostream"
#include "functional"

class Translator {
public:
    Translator();

    void run();

    void startScanning();

private:
    LexicalAnalyzer lex_analyzer_;
    NameChecker varChecker_;
    NameChecker funcChecker_;

    void startState();

    void mainState();

    void definitionState(std::string type);

    void constDefinitionState();

    void functionDefinitionState(std::string type);

    void variableInitializationState(std::string type, std::string id);

    void arrayInitializationState();

    void calculationState();

    void calc8State();

    void calc7State();

    void calc6State();

    void calc5State();

    void calc4State();

    void calc3State();

    void calc2State();

    void calc1State();

    void functionCallState();

    void blockState();

    void operatorState();

    void switchState();

    bool isType(Lexeme lexeme);
};

