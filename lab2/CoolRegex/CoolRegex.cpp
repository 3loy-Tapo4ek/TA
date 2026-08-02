#include "CoolRegex.hpp"

void CoolRegex::compile() {
    Tokenizer tokenizer;
    auto tokens = tokenizer.tokenize(pattern_);
    Parser parser;
    auto ast_root = parser.Parse(tokens);

    FA nfa = NFABuilder().buildNFA(ast_root);
    FA dfa = DFABuilder().buildDFA(nfa);
    min_dfa_ = DFABuilder().buildMinDFA(dfa);
}

void CoolRegex::visualizePipeline(const std::string& prefix) const
{
    Tokenizer tokenizer;
    auto tokens = tokenizer.tokenize(pattern_);

    Parser parser;
    auto ast_root = parser.Parse(tokens);
    DotVisualizer::visualize(ast_root, prefix + "_1_ast.dot");

    NFABuilder nfa_builder;
    FA nfa = nfa_builder.buildNFA(ast_root);
    DotVisualizer::visualize(nfa, prefix + "_2_nfa.dot");

    DFABuilder dfa_builder;
    FA dfa = dfa_builder.buildDFA(nfa);
    DotVisualizer::visualize(dfa, prefix + "_3_dfa.dot");

    FA min_dfa = dfa_builder.buildMinDFA(dfa);
    DotVisualizer::visualize(min_dfa, prefix + "_4_min_dfa.dot");
};

std::vector<std::string> CoolRegex::findAll(const std::string& input)
{
    if (min_dfa_ == std::nullopt) { compile(); }

    std::vector<std::string> results;
    const FA& dfa = min_dfa_.value();
    size_t start_pose = 0;

    while (start_pose < input.size())
    {
        //начинаем идти по автомасьону
        size_t current_id = dfa.start_ptr_;

        std::optional<size_t> last_match_end = std::nullopt;
        for (size_t current_pose = start_pose; current_pose != input.size(); current_pose++)
        {
            char a = input[current_pose];
            bool transition_found = false;

            for (const auto& transition : dfa.states_[current_id].transitions)
            {
                if (transition.symbol.has_value() && transition.symbol.value() == a)
                {
                    current_id = transition.targer_state_id;
        
                    transition_found = true;

                    if (dfa.states_[current_id].is_acceptable_)
                    {
                        last_match_end = current_pose;
                    }
                    break;
                }     
            }
            if (!transition_found)
            {
                break;
            } 
        }

        if (last_match_end.has_value())
        {
            results.push_back(input.substr(start_pose, last_match_end.value() - start_pose + 1));
            start_pose = last_match_end.value() + 1;
        } else {start_pose++;}
    }
    return results;
};
