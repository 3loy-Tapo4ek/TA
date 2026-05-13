#include "NFABuilder.hpp"
#include <stdexcept>

namespace regex_engine {

NFABuilder::NFABuilder(NFA& nfaContext) : nfa_(nfaContext) {}

NFAFragment NFABuilder::getResult() {
    if (stack_.empty()) {
        return {nullptr, nullptr};
    }
    return stack_.top();
}

void NFABuilder::visit(LiteralNode& node) {
    State* start = nfa_.createState();
    State* accept = nfa_.createState();
    start->addTransition(node.character, accept);
    stack_.push({start, accept});
}

void NFABuilder::visit(ConcatNode& node) {
    node.left->accept(*this);
    NFAFragment left = stack_.top(); stack_.pop();
    
    node.right->accept(*this);
    NFAFragment right = stack_.top(); stack_.pop();

    left.accept->addEpsilonTransition(right.start);
    stack_.push({left.start, right.accept});
}

void NFABuilder::visit(UnionNode& node) {
    node.left->accept(*this);
    NFAFragment left = stack_.top(); stack_.pop();
    
    node.right->accept(*this);
    NFAFragment right = stack_.top(); stack_.pop();

    State* start = nfa_.createState();
    State* accept = nfa_.createState();

    start->addEpsilonTransition(left.start);
    start->addEpsilonTransition(right.start);
    left.accept->addEpsilonTransition(accept);
    right.accept->addEpsilonTransition(accept);

    stack_.push({start, accept});
}

void NFABuilder::visit(KleeneNode& node) {
    node.child->accept(*this);
    NFAFragment child = stack_.top(); stack_.pop();

    State* start = nfa_.createState();
    State* accept = nfa_.createState();

    start->addEpsilonTransition(child.start);
    start->addEpsilonTransition(accept);
    child.accept->addEpsilonTransition(child.start);
    child.accept->addEpsilonTransition(accept);

    stack_.push({start, accept});
}

void NFABuilder::visit(OptionalNode& node) {
    node.child->accept(*this);
    NFAFragment child = stack_.top(); stack_.pop();

    State* start = nfa_.createState();
    State* accept = nfa_.createState();

    start->addEpsilonTransition(child.start);
    start->addEpsilonTransition(accept);
    child.accept->addEpsilonTransition(accept);

    stack_.push({start, accept});
}

void NFABuilder::visit(RepeatNode& node) {
    if (node.count <= 0) {
        State* start = nfa_.createState();
        stack_.push({start, start});
        return;
    }

    node.child->accept(*this);
    NFAFragment result = stack_.top(); stack_.pop();

    for (int i = 1; i < node.count; ++i) {
        node.child->accept(*this);
        NFAFragment next = stack_.top(); stack_.pop();
        
        result.accept->addEpsilonTransition(next.start);
        result.accept = next.accept;
    }

    stack_.push(result);
}

void NFABuilder::visit(CaptureGroupNode& node) {
    named_groups_[node.name] = node.child.get();

    node.child->accept(*this);
    NFAFragment child = stack_.top(); stack_.pop();

    State* start = nfa_.createState();
    State* accept = nfa_.createState();

    start->capture_start = node.name;
    accept->capture_end = node.name;

    start->addEpsilonTransition(child.start);
    child.accept->addEpsilonTransition(accept);

    stack_.push({start, accept});
}

void NFABuilder::visit(NameRefNode& node) {
    auto it = named_groups_.find(node.name);
    if (it != named_groups_.end()) {
        it->second->accept(*this);
    } else {
        throw std::runtime_error("Reference to undefined capture group: " + node.name);
    }
}

} // namespace regex_engine