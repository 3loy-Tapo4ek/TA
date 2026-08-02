#include "Parser.hpp"

std::unique_ptr<Node> Parser::Parse(std::vector<Token> tokens)
{
    items_.clear();

    //так называемое многократное сканирование

    //первый шаг - обоваричиваем в скобки и запихиваем все в std::list
    items_.push_back(ParserItem(Token{TokenType::OpenParen}));

    for (auto current_token = tokens.begin(); current_token != tokens.end(); current_token++)
    {
        items_.push_back(ParserItem(*current_token));
    }

    items_.push_back(ParserItem(Token{TokenType::CloseParen}));

    //второй шаг - поиск самых близких скобок
    while (items_.size() > 1)
    {

        auto [start, last] = findClosestParentheses();

        //а) схлопываем ЛИТЕРАЛЫ в а-ноды
        collapseLiterals(start, last);

        //б) схлоппываем ЗАМЫКАНИЕ КЛИНИ в стар-ноды
        collapseStars(start, last);

        //в) схлопываем КОНКАТЕНАЦИЮ в конкат-ноды
        collapseConcat(start, last);

        //г) схлопываем ИЛИ в ор-ноды
        collapseOr(start, last);

        //д) удаляем искуственные скобки
        items_.erase(last);
        items_.erase(start);
    }

    auto root = std::move(items_.front().node_);
    return root;
};


std::pair<std::list<ParserItem>::iterator, std::list<ParserItem>::iterator> Parser::findClosestParentheses()
{
    for (auto current_item = items_.begin(); current_item != items_.end(); current_item++)
    {
        if (current_item->type_ == ParserItem::Type::Token && current_item->token_.token_type_ == TokenType::CloseParen)
        {
            auto start = current_item;
            auto last = current_item;
            while (start != items_.begin() && (start->type_ != ParserItem::Type::Token || start->token_.token_type_ != TokenType::OpenParen))
            {
                start--;
            }

            if (start->token_.token_type_ != TokenType::OpenParen && start == items_.begin())
            {
                throw std::runtime_error("Не закрыли скобки");
            }

            return {start, last};
        }
    }

    throw std::runtime_error("Закончились скобки и внешние скобки обработались неправильно");

}

//Пробегает по всем символам в пределах [start, last] в поисках ЛИТЕРАЛОВ
void Parser::collapseLiterals(std::list<ParserItem>::iterator start, std::list<ParserItem>::iterator last)
{
    for (auto it = std::next(start); it != last; it++)
    {
        if (it->type_ == ParserItem::Type::Token && it->token_.token_type_ == TokenType::Literal)
        {
            char symbol = it->token_.value.value();
            auto node = std::make_unique<ANode>(symbol);
            *it = ParserItem(std::move(node));
        }
    }
};

//Пробегает по всем символам в пределах [start, last] в поисках ЗАМЫКАНИЕ КЛИНИ
void Parser::collapseStars(std::list<ParserItem>::iterator start, std::list<ParserItem>::iterator last)
{
    auto it = std::next(start);

    while (it != last) //т.к. будет erase() удобнее использовать while
    {
        if (it->type_ == ParserItem::Type::Token && it->token_.token_type_ == TokenType::KleeneStar)
        {
            if (std::prev(it)->type_ != ParserItem::Type::Node)
            {
                throw std::runtime_error("Синтаксическая ошибка при написании ЗАМЫКАНИЕ КЛИНИ");
            }

            auto star = std::make_unique<StarNode>(std::move(std::prev(it)->node_));
            *std::prev(it) = ParserItem(std::move(star));

            it = items_.erase(it);
        }

        else { it++; }

    }
};

//Пробегает по всем символам в пределах [start, last] в поисках Конкатенации
void Parser::collapseConcat(std::list<ParserItem>::iterator start, std::list<ParserItem>::iterator last)
{
    auto it = std::next(start);

    while (it != last && std::next(it) != last) //т.к. будет erase() удобнее использовать while
    {
        if (it->type_ == ParserItem::Type::Node && std::next(it)->type_ == ParserItem::Type::Node)
        {
            auto concat = std::make_unique<ConcatNode>(std::move(it->node_), std::move(std::next(it)->node_));
            *it = ParserItem(std::move(concat));

            items_.erase(std::next(it));
        }

        else { it++; }
    }
};

//Пробегает по всем символам в пределах [start, last] в поисках ИЛИ
void Parser::collapseOr(std::list<ParserItem>::iterator start, std::list<ParserItem>::iterator last)
{
    auto it = std::next(start);

    while (it != last && std::next(it) != last) //т.к. будет erase() удобнее использовать while
    {
        if (it->type_ == ParserItem::Type::Token && it->token_.token_type_ == TokenType::Alternation)
        {
            if (std::prev(it)->type_ != ParserItem::Type::Node || std::next(it)->type_ != ParserItem::Type::Node)
            {
                throw std::runtime_error("Синтаксическая ошибка: пустой операнд у оператора '|'");
            }

            auto or_node = std::make_unique<OrNode>(std::move(std::prev(it)->node_), std::move(std::next(it)->node_));
            *(std::prev(it)) = ParserItem(std::move(or_node));

            items_.erase(std::next(it));
            it = items_.erase(it);
        }

        else { it++; }
    }
};


//Быстрая отладка
void Parser::PrintItems()
{
    for (auto current_item = items_.begin(); current_item != items_.end(); current_item++)
    {
        if (current_item->type_ == ParserItem::Type::Token) { std::cout << static_cast<int>(current_item->token_.token_type_); }
        if (current_item->type_ == ParserItem::Type::Node) { std::cout << current_item->node_->ToString(); }

    }
};
