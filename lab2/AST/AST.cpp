#include "AST.hpp"

namespace regex_engine {

// LiteralNode
LiteralNode::LiteralNode(char c) : character(c) {}
void LiteralNode::accept(ASTVisitor& v) { v.visit(*this); }

// ConcatNode
ConcatNode::ConcatNode(std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r) 
    : left(std::move(l)), right(std::move(r)) {}
void ConcatNode::accept(ASTVisitor& v) { v.visit(*this); }

// UnionNode
UnionNode::UnionNode(std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r) 
    : left(std::move(l)), right(std::move(r)) {}
void UnionNode::accept(ASTVisitor& v) { v.visit(*this); }

// KleeneNode
KleeneNode::KleeneNode(std::unique_ptr<ASTNode> c) : child(std::move(c)) {}
void KleeneNode::accept(ASTVisitor& v) { v.visit(*this); }

// OptionalNode
OptionalNode::OptionalNode(std::unique_ptr<ASTNode> c) : child(std::move(c)) {}
void OptionalNode::accept(ASTVisitor& v) { v.visit(*this); }

// RepeatNode
RepeatNode::RepeatNode(std::unique_ptr<ASTNode> c, int count) 
    : child(std::move(c)), count(count) {}
void RepeatNode::accept(ASTVisitor& v) { v.visit(*this); }

// CaptureGroupNode
CaptureGroupNode::CaptureGroupNode(std::string name, std::unique_ptr<ASTNode> c) 
    : name(std::move(name)), child(std::move(c)) {}
void CaptureGroupNode::accept(ASTVisitor& v) { v.visit(*this); }

// NameRefNode
NameRefNode::NameRefNode(std::string name) : name(std::move(name)) {}
void NameRefNode::accept(ASTVisitor& v) { v.visit(*this); }

} // namespace regex_engine