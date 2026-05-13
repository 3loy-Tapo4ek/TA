#pragma once

#include "AST.hpp"
#include "NFA.hpp"
#include <stack>
#include <unordered_map>

namespace regex_engine {

class NFABuilder : public ASTVisitor {
public:
    explicit NFABuilder(NFA& nfaContext);

    NFAFragment getResult();

    void visit(LiteralNode& node) override;
    void visit(ConcatNode& node) override;
    void visit(UnionNode& node) override;
    void visit(KleeneNode& node) override;
    void visit(OptionalNode& node) override;
    void visit(RepeatNode& node) override;
    void visit(CaptureGroupNode& node) override;
    void visit(NameRefNode& node) override;

private:
    NFA& nfa_;
    std::stack<NFAFragment> stack_;
    std::unordered_map<std::string, ASTNode*> named_groups_;
};

} // namespace regex_engine