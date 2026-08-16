// main.cpp
#include <iostream>
#include <fstream>
#include <memory>
#include "Ast.hpp"
#include "parser.tab.hh"

// Глобальный указатель на корень синтаксического дерева
std::unique_ptr<StatementNode> root = nullptr;

extern FILE* yyin;

int main(int argc, char** argv) {
    if (argc > 1) {
        FILE* file = fopen(argv[1], "r");
        if (!file) {
            std::cerr << "Не удалось открыть файл: " << argv[1] << std::endl;
            return 1;
        }
        yyin = file;
    } else {
        std::cout << "Введите программу (в конце нажмите Ctrl+D):" << std::endl;
    }

    yy::parser parser;
    int result = parser.parse();

    if (result == 0 && root != nullptr) {
        std::cout << "\n[УСПЕХ] Парсер успешно построил AST программы!" << std::endl;
    } else {
        std::cout << "\n[ОШИБКА] Синтаксический анализ завершился с ошибкой." << std::endl;
    }

    return result;
}