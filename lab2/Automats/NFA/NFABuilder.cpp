#include "NFABuilder.hpp"

FA NFABuilder::buildNFA(const std::unique_ptr<Node>& root)
{
    states_.clear();
    while (!results_.empty()) results_.pop();

    if (root != nullptr) {
        root->accept(*this);
    }

    FAFragment final_fragment = results_.top();

    return FA{ std::move(states_), final_fragment.start_ptr_, final_fragment.accept_ptr_ };
}

size_t NFABuilder::createState()
{
    size_t id = states_.size();
    states_.push_back(State{id, false, {}});
    return id;
};

//creating FAFragment methods

FAFragment NFABuilder::createLiteralAutomata(char symbol)
{
    size_t start_id = createState();
    size_t accept_id = createState();

    states_[start_id].transitions.push_back(Transition{accept_id, symbol});

    return {start_id, accept_id};
};

FAFragment NFABuilder::createConcatAutomata(const FAFragment& left_automata, const FAFragment& right_automata)
{
    //лишим принимающее состояние левого автомата статуса принимающего
    states_[left_automata.accept_ptr_].is_acceptable_ = false;

    //склеим конец левого автомата с началом правого
    states_[left_automata.accept_ptr_].transitions = std::move(states_[right_automata.start_ptr_].transitions);
    //перенаправим переходы в скленное состояние
    redirectTransitions(right_automata.start_ptr_, left_automata.accept_ptr_);

    return {left_automata.start_ptr_, right_automata.accept_ptr_};
};

//redirects transitions into one state to another
void NFABuilder::redirectTransitions(size_t from_id, size_t to_id)
{
    for (auto state = states_.begin(); state != states_.end(); state++)
    {
        for (auto transition = state->transitions.begin(); transition != state->transitions.end(); transition++)
        {
            if (transition->targer_state_id == from_id) { transition->targer_state_id = to_id; }
        }
    }
};

FAFragment NFABuilder::createOrAutomata(const FAFragment& left_automata, const FAFragment& right_automata)
{
    //лишим принимающее состояние левого и правого автомата статуса принимающего
    states_[left_automata.accept_ptr_].is_acceptable_ = false;
    states_[right_automata.accept_ptr_].is_acceptable_ = false;

    //create new start state
    size_t start_id = createState();

    //create e-transitions from start to left and right automats
    states_[start_id].transitions.push_back(Transition{left_automata.start_ptr_, std::nullopt});
    states_[start_id].transitions.push_back(Transition{right_automata.start_ptr_, std::nullopt});

    //create new accept state
    size_t accept_id = createState();
    states_[accept_id].is_acceptable_ = true;

    //create e-transitions from left and right automats to new accept state
    states_[left_automata.accept_ptr_].transitions.push_back(Transition{accept_id, std::nullopt});
    states_[right_automata.accept_ptr_].transitions.push_back(Transition{accept_id, std::nullopt});

    return {start_id, accept_id};
};

FAFragment NFABuilder::createStarAutomata(const FAFragment& automata)
{
    //лишим принимающее состояние статуса принимающего
    states_[automata.accept_ptr_].is_acceptable_ = false;

    //create new start and accept state
    size_t start_id = createState();

    size_t accept_id = createState();
    states_[accept_id].is_acceptable_ = true;

    //create e-transitions
    
    //from start
    states_[start_id].transitions.push_back(Transition{automata.start_ptr_, std::nullopt});
    states_[start_id].transitions.push_back(Transition{accept_id, std::nullopt});

    //from automata accept
    states_[automata.accept_ptr_].transitions.push_back(Transition{automata.start_ptr_, std::nullopt});
    states_[automata.accept_ptr_].transitions.push_back(Transition{accept_id, std::nullopt});


    return {start_id, accept_id};
};

//Building whole automata

//visitor for ANODE
void NFABuilder::visit(const ANode& node)
{
    results_.push(createLiteralAutomata(node.getSymbol()));
};

//visitor for STAR-node
void NFABuilder::visit(const StarNode& node)
{
    node.getChild()->accept(*this);

    //берем ребенка из стэка и делаем pop()
    auto child_frag = results_.top();
    results_.pop();

    //кладем в стек полученный автомат для замыкания клини
    results_.push(createStarAutomata(child_frag));
};

//visitor for CONCAT node
void NFABuilder::visit(const ConcatNode& node)
{
    node.getLeftChild()->accept(*this);
    node.getRightChild()->accept(*this);

    //get right fragment from stack and pop
    auto right_frag = results_.top();
    results_.pop();

    //get left fragment from stack and pop
    auto left_frag = results_.top();
    results_.pop();

    results_.push(createConcatAutomata(left_frag, right_frag));
};

//visitor for OR-node
void NFABuilder::visit(const OrNode& node)
{
    node.getLeftChild()->accept(*this);
    node.getRightChild()->accept(*this);

    //get right fragment from stack and pop
    auto right_frag = results_.top();
    results_.pop();

    //get left fragment from stack and pop
    auto left_frag = results_.top();
    results_.pop();

    results_.push(createOrAutomata(left_frag, right_frag));
};