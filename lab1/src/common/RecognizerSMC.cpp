#include "Recognizer.hpp"

bool RecognizerSMC::IsType(const Token& token) const { return token.type_.name_ == "TYPE"; }
bool RecognizerSMC::IsName(const Token& token) const { return token.type_.name_ == "NAME"; }
bool RecognizerSMC::IsAssign(const Token& token) const { return token.type_.name_ == "ASSIGN"; }
bool RecognizerSMC::IsLit(const Token& token) const { return token.type_.name_ == "LIT"; }
bool RecognizerSMC::IsOperator(const Token& token) const { 
        return token.type_.name_ == "PLUS" || token.type_.name_ == "MINUS" || 
               token.type_.name_ == "MULTIPLY" || token.type_.name_ == "DIV"; 
            }
bool RecognizerSMC::IsDelim(const Token& token) const { return token.type_.name_ == "DELIM"; }