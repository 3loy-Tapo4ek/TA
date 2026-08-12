#pragma once

#include "ParserItem.hpp"
#include <list>
#include <vector>
#include <unordered_map>
#include <iostream>

#include "Node.hpp"
#include "ANode.hpp"
#include "ConcatNode.hpp"
#include "OrNode.hpp"
#include "StarNode.hpp"

class Parser
{
private:
    std::list<ParserItem> items_;
    std::unordered_map<std::string, std::unique_ptr<Node>> captured_groups_;

    //methods for multiple scannig
    std::pair<std::list<ParserItem>::iterator, std::list<ParserItem>::iterator> findClosestParentheses();
    bool isOperParen(const ParserItem& item);


    void collapseLiterals(std::list<ParserItem>::iterator start, std::list<ParserItem>::iterator last);

    //postfix nodes
    void collapsePostfix(std::list<ParserItem>::iterator start, std::list<ParserItem>::iterator last);
    void handleKleene(auto& it);
    void handleQuestion(auto& it);
    void handleRepeat(auto& it);


    void collapseConcat(std::list<ParserItem>::iterator start, std::list<ParserItem>::iterator last);
    void collapseOr(std::list<ParserItem>::iterator start, std::list<ParserItem>::iterator last);

    void collapseNamedGroupRef(std::list<ParserItem>::iterator start, std::list<ParserItem>::iterator last);
    void saveNamedGroup(std::list<ParserItem>::iterator start, std::list<ParserItem>::iterator last);


public:
    std::unique_ptr<Node> Parse(std::vector<Token> tokens);
    void PrintItems();
};