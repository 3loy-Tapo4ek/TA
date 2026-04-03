cmake_minimum_required(VERSION 3.10)
project(MyProject VERSION 1.0 LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

find_package(FLEX REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FLEX_LEXER_OUT "${CMAKE_CURRENT_BINARY_DIR}/recognizer_flex_generated.cpp")
add_custom_command(
    OUTPUT "${FLEX_LEXER_OUT}"
    COMMAND "${FLEX_EXECUTABLE}" -o "${FLEX_LEXER_OUT}" "${CMAKE_CURRENT_SOURCE_DIR}/src/flex/recognizer_flex.l"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/flex/recognizer_flex.l"
    COMMENT "Flex: recognizer_flex.l -> recognizer_flex_generated.cpp"
    VERBATIM
)

set(SOURCES
    src/index.cpp
    src/common/Recognizer.cpp
    src/common/RecognizerFlex.cpp
    src/common/Timing.cpp
    "${FLEX_LEXER_OUT}"
    src/lexer/LexerRegex.cpp
    src/common/RecognizerSMC.cpp
    src/smc/RecognizerSMC_sm.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    src                
    src/common      
    src/lexer
    src/smc
)