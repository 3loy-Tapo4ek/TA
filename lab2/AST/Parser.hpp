#pragma once

#include "ParserItem.hpp"
#include <list>
#include <vector>
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

    //methods for multiple scannig
    std::pair<std::list<ParserItem>::iterator, std::list<ParserItem>::iterator> findClosestParentheses();

    void collapseLiterals(std::list<ParserItem>::iterator start, std::list<ParserItem>::iterator last);
    void collapseStars(std::list<ParserItem>::iterator start, std::list<ParserItem>::iterator last);
    void collapseConcat(std::list<ParserItem>::iterator start, std::list<ParserItem>::iterator last);
    void collapseOr(std::list<ParserItem>::iterator start, std::list<ParserItem>::iterator last);


public:
    std::unique_ptr<Node> Parse(std::vector<Token> tokens);
    void PrintItems();
};