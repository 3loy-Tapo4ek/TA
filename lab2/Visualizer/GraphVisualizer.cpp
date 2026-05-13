#include "GraphVisualizer.hpp"
#include <fstream>
#include <sstream>

namespace regex_engine {

std::string GraphVisualizer::escapeSymbol(char symbol) {
    if (symbol == EPSILON) return "ε";
    if (symbol == '"') return "\\\"";
    if (symbol == '\\') return "\\\\";
    return std::string(1, symbol);
}

void GraphVisualizer::exportNFA(const NFA& nfa, const std::string& filename) {
    std::ofstream out(filename);
    if (!out.is_open()) return;

    out << "digraph NFA {\n";
    out << "  rankdir=LR;\n"; // Слева направо
    out << "  node [shape=circle];\n";

    // Отмечаем стартовое состояние
    if (nfa.startState) {
        out << "  secret_node [style=invis, width=0];\n";
        out << "  secret_node -> " << nfa.startState->id << ";\n";
    }

    // Отмечаем принимающее состояние (двойной круг)
    if (nfa.acceptState) {
        out << "  " << nfa.acceptState->id << " [shape=doublecircle];\n";
    }

    for (const auto& state : nfa.getStates()) {
        // Отрисовка переходов
        for (const auto& t : state->transitions) {
            out << "  " << state->id << " -> " << t.to->id 
                << " [label=\"" << escapeSymbol(t.symbol) << "\"];\n";
        }

        // Если есть именованные группы, добавим подпись к состоянию
        if (!state->capture_start.empty() || !state->capture_end.empty()) {
            out << "  " << state->id << " [color=blue, xlabel=\"" 
                << state->capture_start << state->capture_end << "\"];\n";
        }
    }

    out << "}\n";
    out.close();
}

void GraphVisualizer::exportDFA(const DFA& dfa, const std::string& filename) {
    std::ofstream out(filename);
    if (!out.is_open()) return;

    out << "digraph DFA {\n";
    out << "  rankdir=LR;\n";
    out << "  node [shape=circle];\n";

    if (dfa.startState) {
        out << "  secret_node [style=invis, width=0];\n";
        out << "  secret_node -> " << dfa.startState->id << ";\n";
    }

    for (const auto& state : dfa.getStates()) {
        // Принимающие состояния — двойной круг
        if (state->is_accepting) {
            out << "  " << state->id << " [shape=doublecircle];\n";
        }

        // Отрисовка переходов из мапы
        for (const auto& [symbol, target] : state->transitions) {
            out << "  " << state->id << " -> " << target->id 
                << " [label=\"" << escapeSymbol(symbol) << "\"];\n";
        }
    }

    out << "}\n";
    out.close();
}

} // namespace regex_engine