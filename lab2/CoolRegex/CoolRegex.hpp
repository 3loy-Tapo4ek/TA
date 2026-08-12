#pragma once

//Кому-то эта библиотека может показаться... крутой...

#include "DotVisualizer.hpp"
#include "Tokenizer.hpp"
#include "Parser.hpp"
#include "NFABuilder.hpp"
#include "DFABuilder.hpp"
#include <string>
#include <vector>
#include <optional>

#include "FAOperator.hpp"

class CoolRegex
{
private:
    std::string pattern_;

    std::optional<FA> min_dfa_;

public:
    explicit CoolRegex(const std::string& pattern) : pattern_(pattern) {};
    explicit CoolRegex(FA dfa) : min_dfa_(std::move(dfa)) {};

    ~CoolRegex() = default;

    void compile();

    static CoolRegex compile(const std::string& pattern) {
        CoolRegex regex(pattern);
        regex.compile();
        return regex;
    }

    std::vector<std::string> findAll(const std::string& input);

    static std::vector<std::string> findAll(const std::string& pattern, const std::string& input) {
        auto regex = CoolRegex::compile(pattern);
        return regex.findAll(input);
    }

    CoolRegex complement() const
    {
        FA comp_fa = FAOperator::MakeComplement(getDFA());
        return CoolRegex(std::move(comp_fa));
    }


    //отладка и пр пр
    void visualizePipeline(const std::string& prefix) const;

    const FA& getDFA() const
    { 
        if (!min_dfa_.has_value()) {
            throw std::runtime_error("Автомат еще не скомпилирован!");
        }
    return min_dfa_.value(); 
    }

    const std::string getPattern() const { return pattern_;}
};