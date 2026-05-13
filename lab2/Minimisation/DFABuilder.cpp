#include "DFABuilder.hpp"
#include <queue>
#include <algorithm>
#include <map>

namespace regex_engine {

std::set<char> DFABuilder::getAlphabet(const NFA& nfa) {
    std::set<char> alphabet;
    for (const auto& state : nfa.getStates()) {
        for (const auto& transition : state->transitions) {
            if (transition.symbol != EPSILON) {
                alphabet.insert(transition.symbol);
            }
        }
    }
    return alphabet;
}

std::set<int> DFABuilder::epsilonClosure(const std::set<int>& start_states, const NFA& nfa) {
    std::set<int> closure = start_states;
    std::queue<int> q;
    for (int id : start_states) q.push(id);

    const auto& all_states = nfa.getStates();

    while (!q.empty()) {
        int current_id = q.front();
        q.pop();

        State* current_state = all_states[current_id].get();
        for (const auto& transition : current_state->transitions) {
            if (transition.symbol == EPSILON) {
                if (closure.find(transition.to->id) == closure.end()) {
                    closure.insert(transition.to->id);
                    q.push(transition.to->id);
                }
            }
        }
    }
    return closure;
}

std::set<int> DFABuilder::move(const std::set<int>& states, char symbol, const NFA& nfa) {
    std::set<int> result;
    const auto& all_states = nfa.getStates();

    for (int id : states) {
        State* current_state = all_states[id].get();
        for (const auto& transition : current_state->transitions) {
            if (transition.symbol == symbol) {
                result.insert(transition.to->id);
            }
        }
    }
    return result;
}

// 1. Построение ДКА (Subset Construction) - Слайд 36
std::unique_ptr<DFA> DFABuilder::build(const NFA& nfa) {
    auto dfa = std::make_unique<DFA>();
    if (!nfa.startState) return dfa;

    std::set<char> alphabet = getAlphabet(nfa);
    
    // Мапа для отслеживания созданных состояний ДКА: Множество ID НКА -> Состояние ДКА
    std::map<std::set<int>, DFAState*> dfa_states_map;
    std::queue<std::set<int>> unmarked_states;

    // Инициализация (Шаг 1)
    std::set<int> initial_nfa_set = {nfa.startState->id};
    std::set<int> start_closure = epsilonClosure(initial_nfa_set, nfa);
    
    bool start_is_accepting = start_closure.find(nfa.acceptState->id) != start_closure.end();
    DFAState* start_dfa_state = dfa->createState(start_is_accepting, start_closure);
    dfa->startState = start_dfa_state;
    
    dfa_states_map[start_closure] = start_dfa_state;
    unmarked_states.push(start_closure);

    // Основной цикл (Шаг 2)
    while (!unmarked_states.empty()) {
        std::set<int> current_subset = unmarked_states.front();
        unmarked_states.pop();
        DFAState* current_dfa_state = dfa_states_map[current_subset];

        for (char symbol : alphabet) {
            std::set<int> move_result = move(current_subset, symbol, nfa);
            if (move_result.empty()) continue;

            std::set<int> target_subset = epsilonClosure(move_result, nfa);

            if (dfa_states_map.find(target_subset) == dfa_states_map.end()) {
                // Нашли новое состояние ДКА
                bool is_accepting = target_subset.find(nfa.acceptState->id) != target_subset.end();
                DFAState* new_state = dfa->createState(is_accepting, target_subset);
                dfa_states_map[target_subset] = new_state;
                unmarked_states.push(target_subset);
            }

            // Добавляем переход (Шаг 2.б.2)
            current_dfa_state->addTransition(symbol, dfa_states_map[target_subset]);
        }
    }

    return dfa;
}

// 2. Минимизация ДКА (Алгоритм разбиения) - Слайд 53
std::unique_ptr<DFA> DFABuilder::minimize(const DFA& dfa) {
    // Реализация алгоритма Хопкрофта (разбиение на группы)
    const auto& original_states = dfa.getStates();
    if (original_states.empty()) return std::make_unique<DFA>();

    // Шаг 1: Разделяем на принимающие (F) и непринимающие (S - F)
    std::vector<std::vector<DFAState*>> partitions(2);
    for (const auto& state : original_states) {
        if (state->is_accepting) partitions[0].push_back(state.get());
        else partitions[1].push_back(state.get());
    }
    if (partitions[1].empty()) partitions.pop_back(); // Если все принимающие
    if (partitions[0].empty()) partitions.erase(partitions.begin()); // Если все непринимающие

    // Собираем алфавит (из переходов ДКА)
    std::set<char> alphabet;
    for (const auto& state : original_states) {
        for (const auto& pair : state->transitions) alphabet.insert(pair.first);
    }

    // Шаг 2 & 3: Итеративное разбиение
    bool changed = true;
    while (changed) {
        changed = false;
        std::vector<std::vector<DFAState*>> new_partitions;

        for (const auto& group : partitions) {
            // Мапа для группировки состояний по их поведению
            // Ключ: вектор ID групп, в которые ведет переход по каждому символу алфавита
            std::map<std::vector<int>, std::vector<DFAState*>> sub_groups;

            for (DFAState* state : group) {
                std::vector<int> signature;
                for (char sym : alphabet) {
                    auto it = state->transitions.find(sym);
                    if (it != state->transitions.end()) {
                        DFAState* target = it->second;
                        // Находим, в какой группе сейчас target
                        int target_group_idx = -1;
                        for (size_t i = 0; i < partitions.size(); ++i) {
                            if (std::find(partitions[i].begin(), partitions[i].end(), target) != partitions[i].end()) {
                                target_group_idx = static_cast<int>(i);
                                break;
                            }
                        }
                        signature.push_back(target_group_idx);
                    } else {
                        signature.push_back(-1); // Нет перехода (ведет в "мертвое" состояние)
                    }
                }
                sub_groups[signature].push_back(state);
            }

            // Если группа разбилась на подгруппы
            for (auto& pair : sub_groups) {
                new_partitions.push_back(pair.second);
            }
            if (sub_groups.size() > 1) changed = true;
        }
        partitions = new_partitions;
    }

    // Шаг 4: Сборка нового минимального ДКА
    auto min_dfa = std::make_unique<DFA>();
    std::map<DFAState*, DFAState*> old_to_new; // Старое состояние из группы -> Новое состояние

    // Создаем состояния
    for (const auto& group : partitions) {
        // Берем первое состояние как представителя
        DFAState* rep = group.front();
        
        // Объединяем nfa_states всей группы (важно для сохранения инфы о захватах)
        std::set<int> merged_nfa_states;
        for(auto s : group) {
             merged_nfa_states.insert(s->nfa_states.begin(), s->nfa_states.end());
        }

        DFAState* new_state = min_dfa->createState(rep->is_accepting, merged_nfa_states);
        
        for (DFAState* s : group) {
            old_to_new[s] = new_state;
            if (s == dfa.startState) {
                min_dfa->startState = new_state;
            }
        }
    }

    // Восстанавливаем переходы
    for (const auto& group : partitions) {
        DFAState* rep = group.front();
        DFAState* current_new_state = old_to_new[rep];

        for (const auto& transition : rep->transitions) {
            char sym = transition.first;
            DFAState* target_old = transition.second;
            current_new_state->addTransition(sym, old_to_new[target_old]);
        }
    }

    return min_dfa;
}

// ... предыдущий код DFABuilder.cpp ...


// Вспомогательная функция: собирает алфавит из переходов ДКА
std::set<char> DFABuilder::getDFAAlphabet(const DFA& dfa) {
    std::set<char> alphabet;
    for (const auto& state : dfa.getStates()) {
        for (const auto& transition : state->transitions) {
            alphabet.insert(transition.first);
        }
    }
    return alphabet;
}

// 1. Операция Дополнения (Complement)
std::unique_ptr<DFA> DFABuilder::buildComplement(const DFA& dfa) {
    auto comp_dfa = std::make_unique<DFA>();
    if (!dfa.startState) return comp_dfa;

    std::set<char> alphabet = getDFAAlphabet(dfa);
    std::map<DFAState*, DFAState*> old_to_new;

    // Шаг 1: Копируем все состояния с инверсией статуса приема
    for (const auto& state : dfa.getStates()) {
        // Заметь: !state->is_accepting
        DFAState* new_state = comp_dfa->createState(!state->is_accepting, state->nfa_states);
        old_to_new[state.get()] = new_state;
        if (state.get() == dfa.startState) {
            comp_dfa->startState = new_state;
        }
    }

    // Создаем "мертвое" состояние на случай неполного ДКА. Оно станет ПРИНИМАЮЩИМ.
    DFAState* dead_state = comp_dfa->createState(true, {}); 
    for (char sym : alphabet) {
        dead_state->addTransition(sym, dead_state); // Петля сама на себя
    }

    // Шаг 2: Копируем переходы. Если перехода нет, направляем в dead_state
    for (const auto& state : dfa.getStates()) {
        DFAState* current_new = old_to_new[state.get()];
        
        for (char sym : alphabet) {
            auto it = state->transitions.find(sym);
            if (it != state->transitions.end()) {
                current_new->addTransition(sym, old_to_new[it->second]);
            } else {
                // Если перехода в оригинале не было, в дополнении он ведет в мертвое состояние
                current_new->addTransition(sym, dead_state);
            }
        }
    }

    return comp_dfa;
}

// 2. Операция Разности (Difference: dfa1 \ dfa2)
std::unique_ptr<DFA> DFABuilder::buildDifference(const DFA& dfa1, const DFA& dfa2) {
    auto diff_dfa = std::make_unique<DFA>();
    if (!dfa1.startState) return diff_dfa;

    // Объединенный алфавит
    std::set<char> alphabet = getDFAAlphabet(dfa1);
    std::set<char> alphabet2 = getDFAAlphabet(dfa2);
    alphabet.insert(alphabet2.begin(), alphabet2.end());

    // Очередь для BFS и мапа для маппинга пар состояний
    std::queue<std::pair<DFAState*, DFAState*>> q;
    std::map<std::pair<DFAState*, DFAState*>, DFAState*> state_map;

    auto start_pair = std::make_pair(dfa1.startState, dfa2.startState);
    
    // Состояние разности принимающее, если q1 - принимающее, а q2 - НЕТ (или q2 вообще не существует / равно nullptr)
    bool is_acc = (start_pair.first && start_pair.first->is_accepting) &&
                  (!start_pair.second || !start_pair.second->is_accepting);
                  
    DFAState* new_start = diff_dfa->createState(is_acc, start_pair.first->nfa_states);
    diff_dfa->startState = new_start;
    
    state_map[start_pair] = new_start;
    q.push(start_pair);

    while (!q.empty()) {
        auto current_pair = q.front();
        q.pop();
        
        DFAState* current_new_state = state_map[current_pair];
        DFAState* s1 = current_pair.first;
        DFAState* s2 = current_pair.second;

        for (char sym : alphabet) {
            DFAState* next1 = nullptr;
            DFAState* next2 = nullptr;

            if (s1 && s1->transitions.find(sym) != s1->transitions.end()) {
                next1 = s1->transitions.at(sym);
            }
            if (s2 && s2->transitions.find(sym) != s2->transitions.end()) {
                next2 = s2->transitions.at(sym);
            }

            // Если оба автомата зашли в тупик - нам не нужен этот путь
            if (!next1 && !next2) continue;

            auto next_pair = std::make_pair(next1, next2);

            if (state_map.find(next_pair) == state_map.end()) {
                // Вычисляем статус приема для нового состояния
                bool next_acc = (next1 && next1->is_accepting) && (!next2 || !next2->is_accepting);
                
                // Для групп захвата сохраняем состояния из первого автомата (dfa1)
                std::set<int> merged_nfa;
                if (next1) merged_nfa = next1->nfa_states;

                DFAState* created_state = diff_dfa->createState(next_acc, merged_nfa);
                state_map[next_pair] = created_state;
                q.push(next_pair);
            }

            current_new_state->addTransition(sym, state_map[next_pair]);
        }
    }

    // После построения разности может образоваться много недостижимых/тупиковых состояний.
    // Рекомендуется прогнать результат через минимизацию.
    return minimize(*diff_dfa);
}

} // namespace regex_engine
