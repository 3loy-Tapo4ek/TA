#pragma once

#include <memory>
#include <string>

namespace regex_engine {

class ASTVisitor;

// Интерфейс узла дерева
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
};

class LiteralNode : public ASTNode {
public:
    char character;
    explicit LiteralNode(char c);
    void accept(ASTVisitor& visitor) override;
};

class ConcatNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    ConcatNode(std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r);
    void accept(ASTVisitor& visitor) override;
};

class UnionNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    UnionNode(std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r);
    void accept(ASTVisitor& visitor) override;
};

class KleeneNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> child;
    explicit KleeneNode(std::unique_ptr<ASTNode> c);
    void accept(ASTVisitor& visitor) override;
};

class OptionalNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> child;
    explicit OptionalNode(std::unique_ptr<ASTNode> c);
    void accept(ASTVisitor& visitor) override;
};

class RepeatNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> child;
    int count;
    RepeatNode(std::unique_ptr<ASTNode> c, int count);
    void accept(ASTVisitor& visitor) override;
};

class CaptureGroupNode : public ASTNode {
public:
    std::string name;
    std::unique_ptr<ASTNode> child;
    CaptureGroupNode(std::string name, std::unique_ptr<ASTNode> c);
    void accept(ASTVisitor& visitor) override;
};

class NameRefNode : public ASTNode {
public:
    std::string name;
    explicit NameRefNode(std::string name);
    void accept(ASTVisitor& visitor) override;
};

// Интерфейс для обхода дерева (Visitor Pattern)
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(LiteralNode& node) = 0;
    virtual void visit(ConcatNode& node) = 0;
    virtual void visit(UnionNode& node) = 0;
    virtual void visit(KleeneNode& node) = 0;
    virtual void visit(OptionalNode& node) = 0;
    virtual void visit(RepeatNode& node) = 0;
    virtual void visit(CaptureGroupNode& node) = 0;
    virtual void visit(NameRefNode& node) = 0;
};

} // namespace regex_engine