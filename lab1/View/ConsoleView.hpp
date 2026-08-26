#pragma once

#include "IRecognizer.hpp"
#include <string>
#include <vector>
#include <iostream>

enum class EngineType
{ 
    Unknown = 0, 
    Regex = 1, 
    Flex = 2, 
    SMC = 3 
};
    
enum class AppMode
{ 
    Unknown = 0, 
    Interactive = 1, 
    File = 2, 
    Benchmark = 3 
};



class ConsoleView
{
public:

    EngineType askEngineChoice() const;
    AppMode askAppMode() const;
    
    std::string askFilename() const;
    
    void printWelcomeMessage() const;
    void printInteractivePrompt() const;
    void printParseResult(const std::string& line, bool is_success) const;
    
    //krasiviy view table
    void printDeclaredVariables(const std::vector<Variable>& variables) const;
    
    //error view
    void printError(const std::string& error_msg) const;
};