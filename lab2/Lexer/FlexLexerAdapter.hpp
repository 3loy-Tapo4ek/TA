#pragma once

#include "ILexer.hpp"
#include <FlexLexer.h>
#include <memory>

namespace regex_engine {

class FlexLexerAdapter : public ILexer {
public:
    explicit FlexLexerAdapter(std::istream& input);
    Token getNextToken() override;

private:
    std::unique_ptr<yyFlexLexer> lexer_;
};

} // namespace regex_engine