cmake_minimum_required(VERSION 3.28)

set(VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(regex_library VERSION ${VERSION} LANGUAGES CXX
        DESCRIPTION "Regular Expression Compiler and Matcher Laboratory Work"
)

set(BIN regex_processor)

set(SOURCES
        main.cpp
        Lexer/Tokenizer.cpp
        Lexer/Tokenizer.hpp
        Lexer/TokenType.hpp
        AST/Nodes/ANode.hpp
        AST/Nodes/ConcatNode.hpp
        AST/Nodes/Node.hpp
        AST/Nodes/OrNode.hpp
        AST/Nodes/StarNode.hpp
        AST/Parser.hpp
        AST/ParserItem.hpp
        AST/Parser.cpp
        AST/Visitor/INodeVisitor.hpp 
        Automats/AutomataStruct.hpp
        Automats/NFA/NFABuilder.hpp
        Automats/NFA/NFABuilder.cpp
        Visualization/DotVisualizer.hpp
        Visualization/DotVisualizer.cpp
        Automats/DFA/DFABuilder.hpp
        Automats/DFA/DFABuilder.cpp
        CoolRegex/CoolRegex.hpp
        CoolRegex/CoolRegex.cpp
        Automats/FAOperator/FAOperator.cpp
        Automats/FAOperator/FAOperator.hpp

)

add_executable(${BIN} ${SOURCES})

target_include_directories(${BIN} PRIVATE
        "${CMAKE_SOURCE_DIR}/"
        "${CMAKE_SOURCE_DIR}/Lexer/"
        "${CMAKE_SOURCE_DIR}/AST/"
        "${CMAKE_SOURCE_DIR}/AST/Nodes/"
        "${CMAKE_SOURCE_DIR}/AST/Visitor/"
        "${CMAKE_SOURCE_DIR}/Automats/"
        "${CMAKE_SOURCE_DIR}/Automats/NFA/"
        "${CMAKE_SOURCE_DIR}/Automats/DFA/"
        "${CMAKE_SOURCE_DIR}/Visualization"
        "${CMAKE_SOURCE_DIR}/CoolRegex"
        "${CMAKE_SOURCE_DIR}/Automats/FAOperator/"
)