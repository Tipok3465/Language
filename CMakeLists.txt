cmake_minimum_required(VERSION 3.26)
project(Language)

set(CMAKE_CXX_STANDARD 17)
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
        src/back/LexicalAnalyzer.cpp)