cmake_minimum_required(VERSION 3.28)

set(VERSION 1.0)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(regex_engine VERSION ${VERSION} LANGUAGES C CXX
        DESCRIPTION "Automata-based Regular Expression Library"
)

# Опция для сборки тестового примера
option(BUILD_TEST "Build test executable" ON)

# Поиск Flex (необходим для генерации лексера)
find_package(FLEX REQUIRED)

# Путь к сгенерированному файлу лексера
set(LEXER_SRC "${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp")
FLEX_TARGET(MyLexer Lexer/lexer.l ${LEXER_SRC})

# Группировка исходных файлов по модулям (аналог COMMON_SOURCES)
set(LIB_SOURCES
        AST/AST.cpp
        Graph/NFA.cpp
        Graph/NFABuilder.cpp
        K_path/RegexRestorer.cpp
        Minimisation/DFA.cpp
        Minimisation/DFABuilder.cpp
        Parser/Parser.cpp
        Regex/Regex.cpp
        Regex/RegexMatch.cpp
        Lexer/FlexLexerAdapter.cpp
        Visualizer/GraphVisualizer.cpp
        ${FLEX_MyLexer_OUTPUTS}
)

# Создание статической библиотеки
add_library(regex_lib STATIC ${LIB_SOURCES})

# Настройка путей инклудов для библиотеки
target_include_directories(regex_lib PUBLIC
        "${CMAKE_SOURCE_DIR}/AST/"
        "${CMAKE_SOURCE_DIR}/Graph/"
        "${CMAKE_SOURCE_DIR}/K_path/"
        "${CMAKE_SOURCE_DIR}/Lexer/"
        "${CMAKE_SOURCE_DIR}/Minimisation/"
        "${CMAKE_SOURCE_DIR}/Parser/"
        "${CMAKE_SOURCE_DIR}/Regex/"
        "${CMAKE_SOURCE_DIR}/Token/"
        "${CMAKE_SOURCE_DIR}/Visualizer/"
        "${CMAKE_CURRENT_BINARY_DIR}/"
)

# Сборка тестового исполняемого файла (аналог CLIENT_BIN / SERVER_BIN)
if (BUILD_TEST)
    set(TEST_BIN regex_app)
    set(TEST_SOURCES
            main.cpp
    )

    add_executable(${TEST_BIN} ${TEST_SOURCES})
    target_link_libraries(${TEST_BIN} PRIVATE regex_lib)

    # Использование тех же директорий для теста
    target_include_directories(${TEST_BIN} PRIVATE
            "${CMAKE_SOURCE_DIR}/AST/"
            "${CMAKE_SOURCE_DIR}/Graph/"
            "${CMAKE_SOURCE_DIR}/K_path/"
            "${CMAKE_SOURCE_DIR}/Lexer/"
            "${CMAKE_SOURCE_DIR}/Minimisation/"
            "${CMAKE_SOURCE_DIR}/Parser/"
            "${CMAKE_SOURCE_DIR}/Regex/"
            "${CMAKE_SOURCE_DIR}/Token/"
    )
endif()

# Опционально: подключение линтеров, если файлы существуют в проекте
if(EXISTS "${CMAKE_SOURCE_DIR}/clang-tidy.cmake")
    include(clang-tidy.cmake)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/clang-format.cmake")
    include(clang-format.cmake)
endif()