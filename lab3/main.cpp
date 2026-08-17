#include <iostream>
#include <memory>
#include "Ast.hpp"
#include "Maze.hpp"
#include "ConsoleRobotController.hpp"
#include "Interpreter.hpp"
#include "parser.tab.hh" // C++ Bison заголовок

// 1. Определение глобального корня AST:
std::unique_ptr<StatementNode> root = nullptr;

extern FILE* yyin;

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Использование: " << argv[0] << " <program.dsl> <maze.txt>\n";
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        std::cerr << "Не удалось открыть файл скрипта: " << argv[1] << "\n";
        return 1;
    }

    // 2. Вызов C++ парсера:
    yy::parser parser;
    int parse_result = parser.parse();
    fclose(yyin);

    if (parse_result != 0 || !root) {
        std::cerr << "[ОШИБКА] Синтаксический анализ завершился с ошибкой.\n";
        return 1;
    }

    std::cout << "[УСПЕХ] Синтаксический анализ прошел успешно!\n";

    try {
        // 3. Загрузка мира и запуск
        Maze maze = Maze::fromFile(argv[2]);
        ConsoleRobotController robot(std::move(maze));

        Interpreter interpreter(robot);
        interpreter.Interpret(*root);

        std::cout << "\n[УСПЕХ] Выполнение завершено.\n";
    } catch (const std::exception& e) {
        std::cerr << "\n[ОШИБКА ВРЕМЕНИ ВЫПОЛНЕНИЯ] " << e.what() << "\n";
        return 1;
    }

    return 0;
}