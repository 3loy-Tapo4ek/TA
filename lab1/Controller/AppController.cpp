#include "AppController.hpp"


AppController::AppController(ConsoleView& view)
    : view_(view), recognizer_(nullptr) {}

void AppController::run()
{
    view_.printWelcomeMessage();

    if (!setupEngine())
    {
        return;
    }

    AppMode mode = view_.askAppMode();

    switch (mode)
    {
        case AppMode::Interactive:
            runInteractiveMode();
            break;
        case AppMode::File:
            runFileMode();
            break;
        case AppMode::Benchmark:
            runBenchmarkMode();
            break;
        default:
            view_.printError("Invalid operation mode.");
            break;
    }
}

bool AppController::setupEngine()
{
    EngineType engine_choice = view_.askEngineChoice();
    recognizer_ = createRecognizer(engine_choice);
    
    if (!recognizer_)
    {
        view_.printError("Invalid engine choice.");
        return false;
    }
    return true;
}

std::unique_ptr<IRecognizer> AppController::createRecognizer(EngineType choice) const
{
    switch (choice)
    {
        case EngineType::Regex: return std::make_unique<RegexRecognizer>();
        case EngineType::Flex:  return std::make_unique<FlexRecognizer>();
        case EngineType::SMC:   return std::make_unique<SMCRecognizer>();
        default: return nullptr;
    }
}

void AppController::runInteractiveMode()
{
    view_.printInteractivePrompt();
    processStream(std::cin, true);
}

void AppController::runFileMode()
{
    std::string filename = view_.askFilename();
    std::ifstream file(filename);
    
    if (!file.is_open())
    {
        view_.printError("Cannot open file: " + filename);
        return;
    }
    
    processStream(file, false);
}

void AppController::runBenchmarkMode()
{
    std::vector<size_t> test_lengths = {1000, 5000, 10000, 20000, 30000, 40000, 60000, 70000, 80000, 90000, 100000};
    Benchmark::run(test_lengths, 50);
}

void AppController::processStream(std::istream& in, bool is_interactive)
{
    std::string line;
    recognizer_->reset();

    while (true)
    {
        if (is_interactive) std::cout << "> ";
        
        if (!std::getline(in, line)) break;
        if (is_interactive && line == "exit") break;
        if (line.empty()) continue;

        auto result = recognizer_->parseLine(line);
        view_.printParseResult(line, result.first);
    }
    view_.printDeclaredVariables(recognizer_->getCorrectVariables());
}