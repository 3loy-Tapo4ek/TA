#include "ConsoleView.hpp"
#include <iomanip>

void ConsoleView::printWelcomeMessage() const
{
    std::cout << "========================================\n"
              << "Recognizer____________________Recognizer\n"
              << "========================================\n\n";
}

EngineType ConsoleView::askEngineChoice() const
{
    std::cout << "Выберите реализацию распознавателя:\n"
              << "1. std::regex\n"
              << "2. Flex Lexer\n"
              << "3. SMC\n"
              << "Ввод: ";
    
    int choice = 0;
    std::cin >> choice;
    
    if (choice >= 1 && choice <= 3) {
        return static_cast<EngineType>(choice);
    }
    return EngineType::Unknown;
}

AppMode ConsoleView::askAppMode() const
{
    std::cout << "\nВыберите режим работы:\n"
              << "1. Консоль\n"
              << "2. Файл\n"
              << "3. Таймирование\n"
              << "Ввод: ";
    
    int choice = 0;
    std::cin >> choice;
    
    if (choice >= 1 && choice <= 3) {
        return static_cast<AppMode>(choice);
    }
    return AppMode::Unknown;
}

std::string ConsoleView::askFilename() const
{
    std::string filename;
    std::cout << "Введите имя файла: ";
    std::getline(std::cin, filename);
    return filename;
}

void ConsoleView::printInteractivePrompt() const
{
    std::cout << "\n--- Консольный режим ---\n"
              << "Ввод строк для распознавания.\n"
              << "Введите 'exit' для завершения работы.\n"
              << "------------------------\n";
}

void ConsoleView::printParseResult(const std::string& line, bool is_success) const
{
    if (is_success) {
        std::cout << "[ OK ]    " << line << "\n";
    } else {
        std::cout << "[FAIL]    " << line << "\n";
    }
}

void ConsoleView::printDeclaredVariables(const std::vector<Variable>& variables) const
{
    std::cout << "\n=== Переменные ===\n";
    if (variables.empty())
    {
        std::cout << "Переменные для вывода отсутствуют.\n";
    }
    else
    {
        for (const auto& var : variables)
        {
            std::cout << var.name_ << " - " << typeToString(var.type_) << "\n";
        }
    }
    std::cout << "==========================\n\n";
}

void ConsoleView::printError(const std::string& error_msg) const
{
    std::cerr << "Error: " << error_msg << "\n";
}