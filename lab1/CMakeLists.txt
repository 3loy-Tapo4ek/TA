cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

set(VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(regex_library VERSION ${VERSION} LANGUAGES CXX
        DESCRIPTION "Recognizer"
)

find_package(FLEX REQUIRED)
flex_target(MyLexer 
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/Lexer.l 
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer/lex.yy.cc
)

find_program(SMC_EXECUTABLE smc REQUIRED)
find_program(GRAPHVIZ_DOT dot)

set(SMC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/SMC)
set(SMC_INPUT ${SMC_DIR}/Recognizer.sm)
set(SMC_OUTPUT_H ${SMC_DIR}/Recognizer_sm.h)
set(SMC_OUTPUT_CPP ${SMC_DIR}/Recognizer_sm.cpp)
set(SMC_OUTPUT_DOT ${SMC_DIR}/Recognizer_sm.dot)
set(SMC_OUTPUT_PNG ${SMC_DIR}/fsm_graph.png)

add_custom_command(
    OUTPUT ${SMC_OUTPUT_H} ${SMC_OUTPUT_CPP}
    COMMAND ${SMC_EXECUTABLE} -c++ -d ${SMC_DIR} ${SMC_INPUT}
    DEPENDS ${SMC_INPUT}
    COMMENT "Generating State Machine C++ code from ${SMC_INPUT}"
)

if(GRAPHVIZ_DOT)
    add_custom_command(
        OUTPUT ${SMC_OUTPUT_DOT}
        COMMAND ${SMC_EXECUTABLE} -graph -d ${SMC_DIR} ${SMC_INPUT}
        DEPENDS ${SMC_INPUT}
        COMMENT "Generating DOT graph from ${SMC_INPUT}"
    )

    add_custom_command(
    OUTPUT ${SMC_OUTPUT_PNG}
COMMAND ${GRAPHVIZ_DOT} -Tpng -Grankdir=TB -Gnodesep=0.8 ${SMC_OUTPUT_DOT} -o ${SMC_OUTPUT_PNG}
    DEPENDS ${SMC_OUTPUT_DOT}
    COMMENT "Rendering FSM graph using FDP engine"
)
    
    add_custom_target(render_fsm_graph ALL DEPENDS ${SMC_OUTPUT_PNG})
endif()

set(CORE_SOURCES
        Recognizer/IRecognizer.hpp
        Recognizer/RegexRecognizer/RegexRecognizer.hpp
        Recognizer/RegexRecognizer/RegexRecognizer.cpp
        Recognizer/FlexRecognizer/Tokens.hpp
        Recognizer/FlexRecognizer/FlexRecognizer.hpp
        Recognizer/FlexRecognizer/FlexRecognizer.cpp
        Recognizer/SMCRecognizer/SMCRecognizer.hpp
        Recognizer/SMCRecognizer/SMCRecognizer.cpp
        Benchmark/Benchmark.hpp
        Benchmark/Benchmark.cpp
        View/ConsoleView.cpp
        View/ConsoleView.hpp
        Controller/AppController.cpp
        Controller/AppController.hpp
        ${FLEX_MyLexer_OUTPUTS}
        ${SMC_OUTPUT_CPP}
)

set(INCLUDE_DIRS
        "${CMAKE_SOURCE_DIR}/"
        "${CMAKE_SOURCE_DIR}/Recognizer/"
        "${CMAKE_SOURCE_DIR}/Recognizer/RegexRecognizer"
        "${CMAKE_SOURCE_DIR}/Recognizer/FlexRecognizer"
        "${CMAKE_SOURCE_DIR}/Recognizer/SMCRecognizer"
        "${CMAKE_SOURCE_DIR}/SMC"
        "${CMAKE_SOURCE_DIR}/Lexer"
        "${CMAKE_SOURCE_DIR}/Benchmark"
        "${CMAKE_SOURCE_DIR}/View"
        "${CMAKE_SOURCE_DIR}/Controller"
)

add_executable(recognizer main.cpp ${CORE_SOURCES})
target_include_directories(recognizer PRIVATE ${INCLUDE_DIRS})