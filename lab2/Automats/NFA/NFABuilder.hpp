#pragma once

#include "AutomataStruct.hpp"
#include "INodeVisitor.hpp"

#include "Node.hpp"
#include "ANode.hpp"
#include "ConcatNode.hpp"
#include "OrNode.hpp"
#include "StarNode.hpp"

#include <memory>
#include <stack>


class NFABuilder : public INodeVisitor
{
private:
    std::vector<State> states_;
    std::stack<FAFragment> results_;

    size_t createState();
    void redirectTransitions(size_t from_id, size_t to_id);

    //methods for creating automat fragments
    FAFragment createLiteralAutomata(char symbol);
    FAFragment createConcatAutomata(const FAFragment& left, const FAFragment& right);
    FAFragment createOrAutomata(const FAFragment& left_automata, const FAFragment& right_automata);
    FAFragment createStarAutomata(const FAFragment& automata);

    //overriding methods from visitor
    void visit(const ANode& node) override;
    void visit(const ConcatNode& node) override;
    void visit(const OrNode& node) override;
    void visit(const StarNode& node) override;

public:
    NFABuilder() {};
    ~NFABuilder() override = default;

    FA buildNFA(const std::unique_ptr<Node>& root);

    void exportToDOT(const FAFragment& fragment, const std::string& filename) const;

};
