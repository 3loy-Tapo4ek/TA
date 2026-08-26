#pragma once

#include "INodeVisitor.hpp"
#include "Node.hpp"
#include "ANode.hpp"
#include "ConcatNode.hpp"
#include "OrNode.hpp"
#include "StarNode.hpp"

#include "AutomataStruct.hpp"

#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <set>

#include <cstdlib>

class DotVisualizer : public INodeVisitor
{
private:
    std::ofstream file_;
    int id_counter_ = 0;
    int last_id_ = -1;

    // Внутренние приватные методы для обхода дерева
    void visit(const ANode& node) override;
    void visit(const ConcatNode& node) override;
    void visit(const OrNode& node) override;
    void visit(const StarNode& node) override;

    // Приватный метод логики отрисовки Дерева
    void renderAST(const Node* ast_root, const std::string& filename);

    // Приватный метод логики отрисовки НКА
    void renderFA(const FA& nfa, const std::string& filename);

public:
    DotVisualizer() = default;
    ~DotVisualizer() override = default;

    // Публичные перегруженные статические функции
    static void visualize(const Node* ast_root, const std::string& filename);
    static void visualize(const std::unique_ptr<Node>& ast_root, const std::string& filename);
    static void visualize(const FA& nfa, const std::string& filename);

    static bool renderPNG(const std::string& dot_filename, const std::string& png_filename);
};