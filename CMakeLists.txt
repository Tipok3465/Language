cmake_minimum_required(VERSION 3.26)
project(Language)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(Language main.cpp
        src/back/Lexeme.h
        src/back/Translator.h
        src/back/Translator.cpp
        src/back/Bor.h
        src/back/Bor.cpp
        src/back/LexicalAnalyzer.h
        src/back/LexicalAnalyzer.cpp
        src/back/TID.h
        src/back/Error.h
        src/back/SemanticStack.h
        src/back/SemanticStack.cpp
        src/back/NameChecker.h
        src/back/NameChecker.cpp
        src/back/Generator.h
        src/back/Generator.cpp)