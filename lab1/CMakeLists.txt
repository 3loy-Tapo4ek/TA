cmake_minimum_required(VERSION 3.10)
project(MyProject VERSION 1.0 LANGUAGES CXX)

# Требуем C++17 (можно изменить на нужный вам стандарт)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Явно перечисляем все .cpp файлы (так надёжнее, чем file(GLOB))
set(SOURCES
    src/index.cpp
    src/common/Recognizer.cpp
    src/lexer/LexerRegex.cpp
    src/common/RecognizerSMC.cpp
    src/smc/RecognizerSMC_sm.cpp
)

# Создаём исполняемый файл (имя совпадает с именем проекта)
add_executable(${PROJECT_NAME} ${SOURCES})

# Указываем, где искать заголовочные файлы (#include "common/...")
target_include_directories(${PROJECT_NAME} PRIVATE
    src                # чтобы писать #include "common/Recognizer.hpp"
    src/common         # если в Recognizer.cpp используется #include "Token.hpp"
    src/lexer          # аналогично для lexer
    src/smc
)