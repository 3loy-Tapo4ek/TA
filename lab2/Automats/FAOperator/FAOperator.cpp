#include "FAOperator.hpp"

std::set<char> FAOperator::getAlphabet(const FA& fa)
{
    std::set<char> alphabet;
    for (const auto& state : fa.states_)
    {
        for (const auto& transition : state.transitions)
        {
            if (transition.symbol.has_value()) { alphabet.insert(transition.symbol.value());}
        }
    }

    return alphabet;
};

std::string FAOperator::ReconstructRegex(const FA& fa)
{

    size_t k = 0;
    size_t n = fa.states_.size();

    //initiate matrix n*n
    std::vector<std::vector<std::string>> R0;
    R0.resize(n);

    for (auto& vector : R0)
    {
        vector.resize(n);
    }

    //filling R0
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < n; j++)
        {
            R0[i][j] = unitingTheArcs(i, j, fa);
        }
    }

    //induction for k-path

    auto R_prev = R0;

    for (size_t k = 0; k < n; k++)
    {
        auto R_next = R_prev;

        for (size_t i = 0; i < n; i++)
        {
            for (size_t j = 0; j < n; j++)
            {
                //R_next[i][j] = R_prev[i][j] | ( R_prev[i][k] * (R_prev[k][k])* * R_prev[k][j] )

                std::string path_without_k = R_prev[i][j]; //was on previous step

                std::string ik = R_prev[i][k];
                std::string kj = R_prev[k][j];

                if (i == k) { ik = "ε"; }
                if (j == k) { kj = "ε"; }

                std::string path_through_k = combinePath(ik, R_prev[k][k], kj); // проходит через к один или более раз
            
                R_next[i][j] = combineUnion(path_without_k, path_through_k);
            }
        }

        R_prev = std::move(R_next);
    }

    std::string final_regex = "";

    for (size_t j = 0; j < n; j++)
    {
        if (fa.states_[j].is_acceptable_)
        {
            std::string path = R_prev[fa.start_ptr_][j];
            final_regex = combineUnion(final_regex, path);
        }
    }

    return cleanupRegex(final_regex);
};

std::string FAOperator::cleanupRegex(std::string expr)
{
    if (expr == "ε") {return "";}

    replaceAll(expr, "ε|", "");
    replaceAll(expr, "|ε", "");
    replaceAll(expr, "ε", "");
    replaceAll(expr, "||", "|");

    return expr;
};

void FAOperator::replaceAll(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) 
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

std::string FAOperator::combineUnion(const std::string& a, const std::string& b)
{
    if (a.empty()) return b;
    if (b.empty()) return a;
    if (a == b) return a;
    return a + "|" + b;
}


std::string FAOperator::combinePath(std::string i_to_k, std::string k_to_k, std::string k_to_j)
{
    if (i_to_k.empty() || k_to_j.empty()) { return ""; }

    std::string loop = cleanupRegex(k_to_k);

    std::vector<std::string> parts;

    if (i_to_k != "ε") { parts.push_back(wrapIfNeeded(i_to_k)); }

    if (!loop.empty()) 
    {
        parts.push_back("(" + loop + ")...");
    }

    if (k_to_j != "ε") { parts.push_back(wrapIfNeeded(k_to_j)); }

    if (parts.empty()) { return "ε"; }

    std::string expression;
    for (const auto& part : parts)
    {
        expression += part;
    }
    
    return expression;
}

std::string FAOperator::wrapIfNeeded(const std::string& str)
{
    // Если внутри выражения есть '|', оборачиваем в скобки для сохранения приоритета
    if (str.find('|') != std::string::npos)
    {
        return "(" + str + ")";
    }
    return str;
}


std::string FAOperator::unitingTheArcs(size_t i, size_t j, const FA& fa)
{
    std::vector<std::string> parts;

    if (i == j) { parts.push_back("ε"); }

    //filling R0
    for (const auto& transition : fa.states_[i].transitions)
    {
        if (transition.targer_state_id == j && transition.symbol.has_value())
        {
            parts.push_back(std::string(1, transition.symbol.value()));
        }
    }

    std::string expression = "";
    for (size_t p = 0; p < parts.size(); ++p)
    {
        expression += parts[p];
        if (p + 1 < parts.size())
        {
            expression += "|";
        }
    }

    return expression;
}

FA FAOperator::MakeDifference(const FA& fa_1, const FA& fa_2)
{
    std::set<char> common_alphabet = getAlphabet(fa_1);
    std::set<char> alphabet_2 = getAlphabet(fa_2);
    common_alphabet.insert(alphabet_2.begin(), alphabet_2.end()); // Объединение алфавитов

    FA complete_fa_1 = makeComplete(fa_1, common_alphabet);
    FA complete_fa_2 = makeComplete(fa_2, common_alphabet);

    std::set<std::pair<size_t, size_t>> reachable_states = getReachable(complete_fa_1, complete_fa_2, common_alphabet);

    std::map<std::pair<size_t, size_t>, size_t> pair_to_id_map;
    
    size_t current_id = 0;
    for (const auto& pair : reachable_states)
    {
        pair_to_id_map[pair] = current_id++;
    }

    FA result;

    auto start_pair = std::make_pair(complete_fa_1.start_ptr_, complete_fa_2.start_ptr_);
    result.start_ptr_ = pair_to_id_map[start_pair];
    result.states_.resize(reachable_states.size());

    for (const auto& [pair, new_id] : pair_to_id_map)
    {
    bool is_acc = isAcceptableState(OperationType::Difference,
                                    complete_fa_1.states_[pair.first].is_acceptable_,
                                    complete_fa_2.states_[pair.second].is_acceptable_);

    State new_state{new_id, is_acc};

    for (char symbol : common_alphabet)
    {
        size_t n1 = getNextState(complete_fa_1, pair.first, symbol);
        size_t n2 = getNextState(complete_fa_2, pair.second, symbol);

        size_t target_id = pair_to_id_map.at({n1, n2});
        new_state.transitions.push_back(Transition{target_id, symbol});
    }

    result.states_[new_id] = new_state;
}

return result;

};

bool FAOperator::isAcceptableState(OperationType op, bool acc1, bool acc2)
{
    switch (op)
    {
        case OperationType::Difference:
            return acc1 && !acc2;
            
        case OperationType::Intersection:
            return acc1 && acc2;
    }
    return false;
};


std::set<std::pair<size_t, size_t>> FAOperator::getReachable(const FA& fa_1, const FA& fa_2, const std::set<char>& alphabet)
{
    std::set<std::pair<size_t, size_t>> visited;
    std::queue<std::pair<size_t, size_t>> queue;

    std::pair<size_t, size_t> start_pair = {fa_1.start_ptr_, fa_2.start_ptr_};
    visited.insert(start_pair);

    queue.push(start_pair);

    while (!queue.empty())
    {
        std::pair<size_t, size_t> current_pair = queue.front();
        queue.pop();

        for (const auto& symbol : alphabet)
        {
            auto transitions_1 = fa_1.states_[current_pair.first].transitions;
            auto transitions_2 = fa_2.states_[current_pair.second].transitions;

            size_t next_id_1 = getNextState(fa_1, current_pair.first, symbol);
            size_t next_id_2 = getNextState(fa_2, current_pair.second, symbol);

            if (!visited.contains({next_id_1, next_id_2}))
            {
                visited.insert({next_id_1, next_id_2});
                queue.push({next_id_1, next_id_2});
            }
        }
    }

    return visited;
};

size_t FAOperator::getNextState(const FA& fa, size_t state_id, char symbol)
{
    for (const auto& transition : fa.states_[state_id].transitions)
    {
        if (transition.symbol.has_value() && transition.symbol.value() == symbol)
        {
            return transition.targer_state_id;
        }
    }
    throw std::runtime_error("Переход не найден!");
}


FA FAOperator::MakeComplement(const FA& fa)
{
    std::set<char> alphabet = getAlphabet(fa);
    FA result = makeComplete(fa, alphabet);

    for (auto& state : result.states_)
    {
        state.is_acceptable_ = !state.is_acceptable_;
    }

    return result;
};

FA FAOperator::makeComplete(const FA& fa, const std::set<char>& alphabet)
{
    FA result = fa;
    size_t dead_state_id = result.states_.size();
    bool is_dead_state_needed = false;

    for (auto& state : result.states_)
    {
        std::set<char> existing_symbols = getExistingSymbols(state);

        for (const auto& symbol : alphabet)
        {
            if (!existing_symbols.contains(symbol))
            {
                state.transitions.push_back(Transition{dead_state_id, symbol});
                is_dead_state_needed = true;
            }
        }
    }

    if (is_dead_state_needed) {CreateDeadState(result, alphabet, dead_state_id);}

    return result;
};

void FAOperator::CreateDeadState(FA& fa, const std::set<char>& alphabet, size_t& dead_id)
{
    State dead_state{dead_id, false};

    for (char symbol : alphabet)
    {
        dead_state.transitions.push_back(Transition{dead_id, symbol});
    }
    
    fa.states_.push_back(dead_state);
};


std::set<char> FAOperator::getExistingSymbols(const State& state)
{
    std::set<char> symbols;
    for (const auto& tr : state.transitions)
    {
        if (tr.symbol.has_value())
        {
            symbols.insert(tr.symbol.value());
        }
    }
    return symbols;
}

