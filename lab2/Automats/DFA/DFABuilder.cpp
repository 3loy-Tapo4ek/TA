#include "DFABuilder.hpp"

FA DFABuilder::buildDFA(const FA& nfa)
{
    dfa_state_ids_.clear();
    unmarked_dfa_states_ = {};

    FA dfa;
    std::set<char> alphabet = getAlphabet(nfa);

    //1)найдем стартовое состояние ДКА, сделав е-замыкание от начального состояния НКА
    std::set<size_t> start_dfa_state = getEpsilonClosure(nfa, nfa.start_ptr_);
    //add it to map with key 0
    dfa_state_ids_.emplace(start_dfa_state, 0);

    //2)добавим это состояние в очередь не помеченных
    unmarked_dfa_states_.push(start_dfa_state);

    //создаем в ДКА начальное состояние
    bool is_start_accept = start_dfa_state.contains(nfa.accept_ptr_);
    dfa.states_.push_back(State{0, is_start_accept});
    dfa.start_ptr_ = 0;

    //3)проходка по непомеченным состояниям
    while (!unmarked_dfa_states_.empty())
    {
        //a)достаем первое множество состояний и помечаем его
        std::set<size_t> current_dfa_set_of_states = unmarked_dfa_states_.front();
        unmarked_dfa_states_.pop(); 

        //б)для каждого символа алфавита делаем z(m(set))
        for (const auto symbol : alphabet)
        {
            std::set<size_t> next_dfa_set_of_state = getEpsilonClosure(nfa, move(nfa, current_dfa_set_of_states, symbol));
            
            if (next_dfa_set_of_state.empty()) { continue; }
            
            bool is_accept = next_dfa_set_of_state.contains(nfa.accept_ptr_);

            //в)проверяем, было ли состояние обработано
            if (!dfa_state_ids_.contains(next_dfa_set_of_state))
            {
                size_t new_id = dfa_state_ids_.size();
                dfa_state_ids_.emplace(next_dfa_set_of_state, new_id);
                unmarked_dfa_states_.push(next_dfa_set_of_state);

                dfa.states_.push_back(State{new_id, is_accept});
            }

            //г)добавляем переходы
            size_t current_id = dfa_state_ids_[current_dfa_set_of_states];
            size_t next_id = dfa_state_ids_[next_dfa_set_of_state];

            dfa.states_[current_id].transitions.push_back(Transition{next_id, symbol});
        }
    }

    return dfa;
};

FA DFABuilder::buildMinDFA(const FA& dfa)
{
    std::set<char> alphabet = getAlphabet(dfa);
    //1. Разбиение П на две группы П={F, S-F}
    std::set<size_t> set_of_accept_states;
    std::set<size_t> set_of_not_accept_states;

    for (const auto& state : dfa.states_)
    {
        if (state.is_acceptable_ == true) { set_of_accept_states.insert(state.id_); }
        else { set_of_not_accept_states.insert(state.id_); }
    }

    //создаем единый вектор разбиения П = {G0, G1, G2, ...} где G - группа (множество id)
    std::vector<std::set<size_t>> partitions; //П

    //Создаем П0 = {F ; S - F}
    if (!set_of_accept_states.empty()) {partitions.push_back(set_of_accept_states);}
    if (!set_of_not_accept_states.empty()) {partitions.push_back(set_of_not_accept_states);}
    
    //вектор для быстрого понимания, какой группе принадлежит стейт
    //более формально-математически назвал бы это y(s)= i, 
    //где i - номер группы Gi
    std::vector<size_t> state_to_group = buildStateToGroup(dfa.states_.size(), partitions);

    //2.цикл расщепления
    bool is_changed = true;
    while (is_changed)
    {
        is_changed = false;
        std::vector<std::set<size_t>> new_partitions;

        for (const auto& group : partitions)
        {
            //Мапа, в которой
            //КЛЮЧ это rq(q) - вектор результатов дельты
            //ЗНАЧЕНИЕ это множество соотвествующих стейтов
            //кому-то это может показаться крутым... (пистолет-расческа)
            std::map<std::vector<size_t>, std::set<size_t>> map_results_of_delta;

            // Проходим по КАЖДОМУ состоянию из текущей группы
            for (size_t state_id : group)
            {
                //вектор, который будет хранить номера всех групп, в которые
                //можно попасть из стейта по каждому символу алфавита
                //более математически круто: rd(q) = {y(d(s, a1)), y(d(s, a2)) ...}
                std::vector<size_t> results_of_delta = getResultOfDelta(dfa, state_id, alphabet, state_to_group);

                //инсертим в мапу результаты дельты для стейка
                //мапа сама разделит, если группы разнятся
                map_results_of_delta[results_of_delta].insert(state_id);
            }

            for (const auto& [signature, subgroup] : map_results_of_delta)
            {
                new_partitions.push_back(subgroup);
            }

            if (map_results_of_delta.size() > 1 )
            {
                is_changed = true;
            }
        }
    partitions = std::move(new_partitions);

    state_to_group = buildStateToGroup(dfa.states_.size(), partitions);
    }

    return reconstructMinDFA(dfa, partitions, state_to_group);
};

//заменяет группы представителями
FA DFABuilder::reconstructMinDFA(const FA& dfa, const std::vector<std::set<size_t>>& partitions, const std::vector<size_t>& state_to_group)
{
    FA min_dfa;

    for (size_t group_id = 0; group_id != partitions.size(); group_id++)
    {
        State min_state;
        min_state.id_ = group_id;

        bool is_acceptable = false;

        for (size_t state_id : partitions[group_id]) 
        {
            // Если хотя бы одно состояние в группе принимающее — вся группа принимающая
            if (dfa.states_[state_id].is_acceptable_)
            {
                is_acceptable = true;
            }
    
            // Если в этой группе лежит стартовое состояние исходного ДКА
            if (state_id == dfa.start_ptr_)
            {
                min_dfa.start_ptr_ = group_id; // Группа становится стартовой в min_dfa!
            }
        }

        // 1. Берем ID первого состояния из группы (* разыменовывает итератор)
        size_t rep_state_id = *partitions[group_id].begin();

        // 2. Получаем ссылку на само состояние-представитель
        const State& rep_state = dfa.states_[rep_state_id];

        // 3. Проходим по его переходам
        for (const auto& trans : rep_state.transitions) 
        {
            // Узнаем, в КАКУЮ ГРУППУ ведет этот переход
            size_t target_group = state_to_group[trans.targer_state_id];
    
            // Добавляем переход в новое состояние
            min_state.transitions.push_back(Transition{target_group, trans.symbol});
        }

        min_state.is_acceptable_ = is_acceptable;
        min_dfa.states_.push_back(min_state);
    }

    return min_dfa;
};

std::vector<size_t> DFABuilder::getResultOfDelta(const FA& dfa, size_t state_id, const std::set<char>& alphabet, const std::vector<size_t>& state_to_group)
{
    std::vector<size_t> output_vector;
    //заполнеяем вышестоящий вектор
    for (char symbol : alphabet)
    {
        size_t target_group = std::numeric_limits<size_t>::max();

        for (const auto& transition : dfa.states_[state_id].transitions)
        {
            if (transition.symbol == symbol)
            {
                target_group = state_to_group[transition.targer_state_id];                            
                break;
            }
        }

        output_vector.push_back(target_group);
    }
    return output_vector;
};

std::vector<size_t> DFABuilder::buildStateToGroup(size_t num_states, const std::vector<std::set<size_t>>& partitions)
{
    std::vector<size_t> output_vector(num_states);

    //Заполняем вектор G0, G1, G2 ...
    for (size_t group_idx = 0; group_idx < partitions.size(); group_idx++)
    {
        for (size_t state_id : partitions[group_idx])
        {
            output_vector[state_id] = group_idx;
        }
    }

    return output_vector;
};

//обход в ширину для E-замыкания
std::set<size_t> DFABuilder::getEpsilonClosure(const FA& nfa, size_t start_id)
{
    std::set<size_t> closure;    
    std::queue<size_t> queue;

    closure.insert(start_id);

    queue.push(start_id);

    while (!queue.empty())
    {
        size_t current_id = queue.front();
        queue.pop();

        for (const auto& transition : nfa.states_[current_id].transitions)
        {
            if (!closure.contains(transition.targer_state_id) && transition.symbol == std::nullopt)
            {
                closure.insert(transition.targer_state_id);

                queue.push(transition.targer_state_id);
            }
        }
    }

    return closure;
};

//перегрузка бфс для множества состояния
std::set<size_t> DFABuilder::getEpsilonClosure(const FA& nfa, const std::set<size_t>& start_states)
{
    std::set<size_t> closure;
    for (const auto& state : start_states)
    {
        std::set<size_t> current_closure = getEpsilonClosure(nfa, state);
        closure.merge(current_closure);
    }

    return closure;
};

//collects symbols into alphabet
std::set<char> DFABuilder::getAlphabet(const FA& nfa)
{
    std::set<char> alphabet;
    for (const auto& state : nfa.states_)
    {
        for (const auto& transition : state.transitions)
        {
            if (transition.symbol.has_value()) { alphabet.insert(transition.symbol.value());}
        }
    }

    return alphabet;
};

//creates "closure" by exact symbol
std::set<size_t> DFABuilder::move(const FA& nfa, const std::set<size_t>& set_of_states, char symbol)
{
    std::set<size_t> target_states;

    for (const auto& state : set_of_states)
    {
        for (const auto& transition : nfa.states_[state].transitions)
        {
            if (transition.symbol == symbol) { target_states.insert(transition.targer_state_id);}
        }
    }
    return target_states;
};