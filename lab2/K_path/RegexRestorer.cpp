#include "RegexRestorer.hpp"
#include <map>

namespace regex_engine {

// --- Реализация умного выражения RegexExpr ---

RegexExpr::RegexExpr() : str(""), is_empty_set(true), is_epsilon(false), precedence(3) {}

RegexExpr RegexExpr::makeEmptySet() {
    return RegexExpr();
}

RegexExpr RegexExpr::makeEpsilon() {
    RegexExpr e;
    e.is_empty_set = false;
    e.is_epsilon = true;
    e.precedence = 3;
    return e;
}

RegexExpr RegexExpr::makeChar(char c) {
    RegexExpr e;
    e.str = std::string(1, c);
    e.is_empty_set = false;
    e.is_epsilon = false;
    e.precedence = 3; // Атомарное выражение (наивысший приоритет)
    return e;
}

RegexExpr RegexExpr::unite(const RegexExpr& a, const RegexExpr& b) {
    if (a.is_empty_set) return b;
    if (b.is_empty_set) return a;
    if (a.str == b.str) return a; // Упрощение A|A = A

    RegexExpr res;
    res.is_empty_set = false;
    res.is_epsilon = false;
    res.str = a.str + "|" + b.str;
    res.precedence = 0; // Операция ИЛИ имеет низший приоритет
    return res;
}

RegexExpr RegexExpr::concat(const RegexExpr& a, const RegexExpr& b) {
    if (a.is_empty_set || b.is_empty_set) return makeEmptySet();
    if (a.is_epsilon) return b;
    if (b.is_epsilon) return a;

    std::string str_a = a.str;
    std::string str_b = b.str;

    // Расставляем скобки, если склеиваем выражения с оператором ИЛИ
    if (a.precedence < 1) str_a = "(" + str_a + ")";
    if (b.precedence < 1) str_b = "(" + str_b + ")";

    RegexExpr res;
    res.is_empty_set = false;
    res.is_epsilon = false;
    res.str = str_a + str_b;
    res.precedence = 1; // Приоритет конкатенации
    return res;
}

RegexExpr RegexExpr::kleene(const RegexExpr& a) {
    if (a.is_empty_set || a.is_epsilon) return makeEpsilon();

    std::string str_a = a.str;
    // Если внутри не атомарное выражение, берем в скобки
    if (a.precedence < 3) str_a = "(" + str_a + ")";

    RegexExpr res;
    res.is_empty_set = false;
    res.is_epsilon = false;
    // Используем '...' согласно ТЗ задания
    res.str = str_a + "...";
    res.precedence = 2; // Приоритет Замыкания Клини
    return res;
}

// --- Реализация Алгоритма K-путей ---

std::string RegexRestorer::restore(const DFA& dfa) {
    const auto& states = dfa.getStates();
    int n = static_cast<int>(states.size());
    if (n == 0) return "";

    // Создаем маппинг состояний в индексы 0..n-1 для матрицы
    std::map<DFAState*, int> state_to_idx;
    for (int i = 0; i < n; ++i) {
        state_to_idx[states[i].get()] = i;
    }

    // Инициализация 3D массива R[k][i][j], но нам нужны только R_prev и R_curr
    std::vector<std::vector<RegexExpr>> R_prev(n, std::vector<RegexExpr>(n));
    std::vector<std::vector<RegexExpr>> R_curr(n, std::vector<RegexExpr>(n));

    // БАЗИС k = -1 (слайд 6 презентации: объединение всех меток дуг)
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            RegexExpr expr = RegexExpr::makeEmptySet();
            
            // Ищем все переходы из i в j
            DFAState* state_i = states[i].get();
            for (const auto& transition : state_i->transitions) {
                if (state_to_idx[transition.second] == j) {
                    expr = RegexExpr::unite(expr, RegexExpr::makeChar(transition.first));
                }
            }
            
            // Если i == j, добавляем ε (эпсилон)
            if (i == j) {
                expr = RegexExpr::unite(expr, RegexExpr::makeEpsilon());
            }
            
            R_prev[i][j] = expr;
        }
    }

    // ИНДУКЦИЯ: Итерация по разрешенным промежуточным вершинам k от 0 до n-1
    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                // Формула: R[i][j] = R_prev[i][j] U (R_prev[i][k] * R_prev[k][k]... * R_prev[k][j])
                
                RegexExpr term1 = R_prev[i][j];
                
                RegexExpr star_term = RegexExpr::kleene(R_prev[k][k]);
                RegexExpr concat1 = RegexExpr::concat(R_prev[i][k], star_term);
                RegexExpr term2 = RegexExpr::concat(concat1, R_prev[k][j]);
                
                R_curr[i][j] = RegexExpr::unite(term1, term2);
            }
        }
        // Перекладываем для следующей итерации
        R_prev = R_curr;
    }

    // ИТОГ: Объединяем R[start][accept] для всех принимающих состояний
    int start_idx = state_to_idx[dfa.startState];
    RegexExpr final_expr = RegexExpr::makeEmptySet();

    for (int j = 0; j < n; ++j) {
        if (states[j]->is_accepting) {
            final_expr = RegexExpr::unite(final_expr, R_prev[start_idx][j]);
        }
    }

    if (final_expr.is_empty_set) {
        return ""; // Язык пуст
    } else if (final_expr.is_epsilon) {
        return ""; // Принимает только пустую строку (в регулярках это пустая строка)
    }

    return final_expr.str;
}

} // namespace regex_engine