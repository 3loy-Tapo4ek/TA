#include "Tokenizer.hpp"
#include "Parser.hpp"
#include "NFABuilder.hpp"
#include "DFABuilder.hpp"
#include "DotVisualizer.hpp"

#include <iostream>


int main()
{
    Tokenizer test_tokenizer;
    std::vector<Token> tokens = test_tokenizer.tokenize("l(l|d)...");
    DotVisualizer visualizer;

    Parser test_parser;

    //строим СД
    auto ast_root = test_parser.Parse(tokens);
    visualizer.visualize(ast_root, "ast.dot");

    //строим НКА
    NFABuilder builder;
    FA nfa = builder.buildNFA(ast_root); // Получили чистую структуру НКА

    // Передаем НКА в отдельный визуализатор
    visualizer.visualize(nfa, "nfa.dot");

    //Построим ДКА из НКА
    DFABuilder dfabuilder;
    FA dfa = dfabuilder.buildDFA(nfa);

    //визуализация ДКА
    visualizer.visualize(dfa, "dfa.dot");

    //построим минДКА
    FA min_dfa = dfabuilder.buildMinDFA(dfa);
    visualizer.visualize(min_dfa, "min_dfa.dot");

    std::cout << std::endl;;

    return 0;
}