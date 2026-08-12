cmake_minimum_required(VERSION 3.28)

set(VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(regex_library VERSION ${VERSION} LANGUAGES CXX
        DESCRIPTION "Regular Expression Compiler and Matcher Laboratory Work"
)

find_package(Catch2 REQUIRED)

set(CORE_SOURCES
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

set(INCLUDE_DIRS
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

add_executable(regex_processor main.cpp ${CORE_SOURCES})
target_include_directories(regex_processor PRIVATE ${INCLUDE_DIRS})

add_executable(run_tests test.cpp ${CORE_SOURCES})
target_include_directories(run_tests PRIVATE ${INCLUDE_DIRS})
target_link_libraries(run_tests PRIVATE Catch2::Catch2)