#include "Recognizer.hpp"
#include "iostream"

std::vector<std::pair<bool, std::string>> RecognizerSMC::TakeStatistics()
{
        Stats output_statistics;

        context_.enterStartState();

        for (const auto& it : tokens_)
        {
                context_.process(it, output_statistics);
        }

        Token endOfFileToken;
        endOfFileToken.name_ = "\n"; 
        endOfFileToken.type_.name_ = "DELIM";

        context_.process(endOfFileToken, output_statistics);

        return output_statistics.output_;
}



void RecognizerSMC::pose_increment(const Token& token, Stats& stats)
{
        if (!stats.buffer_.empty()) stats.buffer_ += " ";
        stats.buffer_ += token.name_;

        pose_++;
}

void RecognizerSMC::pose_restart(const Token& token, Stats& stats)
{    
    stats.output_.push_back({true, stats.buffer_});
    
    stats.buffer_.clear();
}

void RecognizerSMC::alarm(const Token& token, Stats& stats)
{

    stats.buffer_ += token.name_;
    pose_++;
    
    stats.output_.push_back({false, stats.buffer_});
    
    stats.buffer_.clear();
}

void RecognizerSMC::restart_from_error(const Token& token, Stats& stats)
{

    stats.buffer_.clear();

    stats.buffer_ += token.name_; 
}

bool RecognizerSMC::IsType(const Token& token) const { return token.type_.name_ == "TYPE"; }
bool RecognizerSMC::IsName(const Token& token) const { return token.type_.name_ == "NAME"; }
bool RecognizerSMC::IsAssign(const Token& token) const { return token.type_.name_ == "ASSIGN"; }
bool RecognizerSMC::IsLit(const Token& token) const { return token.type_.name_ == "LIT"; }
bool RecognizerSMC::IsOperator(const Token& token) const { 
        return token.type_.name_ == "PLUS" || token.type_.name_ == "MINUS" || 
               token.type_.name_ == "MULTIPLY" || token.type_.name_ == "DIV"; 
            }
bool RecognizerSMC::IsDelim(const Token& token) const { return token.type_.name_ == "DELIM"; }