#include "DotVisualizer.hpp"

// Реализация приватных методов-посетителей для Дерева
void DotVisualizer::visit(const ANode& node)
{
    int id = id_counter_++;
    
    // Безопасно формируем подпись для Graphviz
    std::string label = node.getSymbol().has_value() 
        ? std::string(1, node.getSymbol().value()) 
        : "ε";

    file_ << "    node" << id << " [label=\"" << label 
          << "\", shape=circle, style=filled, fillcolor=\"#C8E6C9\"];\n";
          
    last_id_ = id;
}

void DotVisualizer::visit(const StarNode& node)
{
    int id = id_counter_++;
    file_ << "    node" << id << " [label=\"...\", shape=circle, style=filled, fillcolor=\"#FFF9C4\"];\n";
    
    node.getChild()->accept(*this);
    int child_id = last_id_;

    file_ << "    node" << id << " -> node" << child_id << ";\n";
    last_id_ = id;
}

void DotVisualizer::visit(const ConcatNode& node)
{
    int id = id_counter_++;
    file_ << "    node" << id << " [label=\"*\", shape=circle, style=filled, fillcolor=\"#BBDEFB\"];\n";

    node.getLeftChild()->accept(*this);
    int left_id = last_id_;

    node.getRightChild()->accept(*this);
    int right_id = last_id_;

    file_ << "    node" << id << " -> node" << left_id << ";\n";
    file_ << "    node" << id << " -> node" << right_id << ";\n";
    last_id_ = id;
}

void DotVisualizer::visit(const OrNode& node)
{
    int id = id_counter_++;
    file_ << "    node" << id << " [label=\"|\", shape=circle, style=filled, fillcolor=\"#FFCDD2\"];\n";

    node.getLeftChild()->accept(*this);
    int left_id = last_id_;

    node.getRightChild()->accept(*this);
    int right_id = last_id_;

    file_ << "    node" << id << " -> node" << left_id << ";\n";
    file_ << "    node" << id << " -> node" << right_id << ";\n";
    last_id_ = id;
}

// Внутренняя логика отрисовки AST
void DotVisualizer::renderAST(const Node* ast_root, const std::string& filename)
{
    file_.open(filename);
    file_ << "digraph AST {\n    node [fontname=\"Arial\"];\n";

    if (ast_root != nullptr) {
        id_counter_ = 0;
        last_id_ = -1;
        ast_root->accept(*this);
    }

    file_ << "}\n";
    file_.close();
}

// Внутренняя логика отрисовки НКА
// Внутренняя логика отрисовки автомата (подходит и для НКА, и для ДКА)
void DotVisualizer::renderFA(const FA& fa, const std::string& filename)
{
    std::ofstream file(filename);
    file << "digraph FA {\n    rankdir=LR;\n    fontname=\"Arial\";\n\n";

    // 1. Отрисовка принимающих состояний (двойной зеленый круг)
    file << "    node [shape=doublecircle, style=filled, fillcolor=\"#C8E6C9\"];\n";
    for (const auto& state : fa.states_) {
        if (state.is_acceptable_) {
            file << "    s" << state.id_ << " [label=\"" << state.id_ << "\"];\n";
        }
    }

    // 2. Отрисовка обычных состояний (одинарный белый круг)
    file << "    node [shape=circle, style=filled, fillcolor=\"#FFFFFF\"];\n";
    for (const auto& state : fa.states_) {
        if (!state.is_acceptable_) {
            file << "    s" << state.id_ << " [label=\"" << state.id_ << "\"];\n";
        }
    }

    // 3. Указываем входное (начальное) состояние
    file << "\n    node [shape=none, label=\"\"]; __start__;\n";
    file << "    __start__ -> s" << fa.start_ptr_ << ";\n\n";

    // 4. Отрисовка всех переходов между состояниями
    for (const auto& state : fa.states_) {
        for (const auto& trans : state.transitions) {
            file << "    s" << state.id_ << " -> s" << trans.targer_state_id;
            
            if (trans.symbol.has_value()) {
                file << " [label=\"" << trans.symbol.value() << "\"];\n";
            } else {
                file << " [label=\"&#949;\"];\n"; // Эпсилон (ε)
            }
        }
    }

    file << "}\n";
    file.close();
}

// Публичные перегрузки
void DotVisualizer::visualize(const Node* ast_root, const std::string& filename)
{
    DotVisualizer instance;
    instance.renderAST(ast_root, filename);

    std::string png_filename = filename.substr(0, filename.find_last_of('.')) + ".png";
    renderPNG(filename, png_filename);
}

void DotVisualizer::visualize(const std::unique_ptr<Node>& ast_root, const std::string& filename)
{
    visualize(ast_root.get(), filename);

    std::string png_filename = filename.substr(0, filename.find_last_of('.')) + ".png";
    renderPNG(filename, png_filename);
}

void DotVisualizer::visualize(const FA& nfa, const std::string& filename)
{
    DotVisualizer instance;
    instance.renderFA(nfa, filename);

    std::string png_filename = filename.substr(0, filename.find_last_of('.')) + ".png";
    renderPNG(filename, png_filename);
}

bool DotVisualizer::renderPNG(const std::string& dot_filename, const std::string& png_filename)
{
    // Формируем консольную команду: dot -Tpng ast.dot -o ast.png
    std::string command = "dot -Tpng " + dot_filename + " -o " + png_filename;
    
    // Запускаем команду в операционной системе
    int result = std::system(command.c_str());

    if (result != 0) {
        std::cerr << "Предупреждение: не удалось сгенерировать " << png_filename 
                  << ". Убедитесь, что утилита 'dot' (Graphviz) установлена в системе." << std::endl;
        return false;
    }

    std::cout << "Успешно создано изображение: " << png_filename << std::endl;
    return true;
}