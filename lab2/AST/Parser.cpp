#include "Parser.hpp"
#include <iostream>

std::unique_ptr<Node> Parser::Parse(std::vector<Token> tokens)
{
    items_.clear();
    captured_groups_.clear();

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
        //a1) схлопываем ГРУППЫ_ЗАХВАТА
        collapseNamedGroupRef(start, last);


        //б) схлоппываем ПОСТФИКСНЫЕ метасимволы
        collapsePostfix(start, last);

        //в) схлопываем КОНКАТЕНАЦИЮ в конкат-ноды
        collapseConcat(start, last);

        //г) схлопываем ИЛИ в ор-ноды
        collapseOr(start, last);

        //if oper paren was a NAMED_GROUP - save `em to map
        saveNamedGroup(start, last);

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
            while (start != items_.begin() && !isOperParen(*start))
            {
                start--;
            }

            if (!isOperParen(*start))
            {
                throw std::runtime_error("Не закрыли скобки");
            }

            return {start, last};
        }
    }

    throw std::runtime_error("Закончились скобки и внешние скобки обработались неправильно");
}

bool Parser::isOperParen(const ParserItem& item)
{
    return item.type_ == ParserItem::Type::Token &&
    (item.token_.token_type_ == TokenType::OpenParen || 
    item.token_.token_type_ == TokenType::NamedGroupName);
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

void Parser::collapseNamedGroupRef(std::list<ParserItem>::iterator start, std::list<ParserItem>::iterator last)
{
    for (auto it = std::next(start); it != last; it++)
    {
        if (it->type_ == ParserItem::Type::Token && it->token_.token_type_ == TokenType::NamedGroupRef)
        {
            std::string name = it->token_.group_name.value();
            std::cout << "[PARSER] LOOKING UP GROUP: '" << name << "'\n";
            if (!captured_groups_.contains(name))
            {
                throw std::runtime_error("Неизвестная группа: " + name);
            }

            *it = ParserItem(captured_groups_[name]->clone());
        }
    }
};


//Пробегает по всем символам в пределах [start, last] в поисках ЗАМЫКАНИЕ КЛИНИ или вопросика
void Parser::collapsePostfix(std::list<ParserItem>::iterator start, std::list<ParserItem>::iterator last)
{
    auto it = std::next(start);

    while (it != last) //т.к. будет erase() удобнее использовать while
    {
        if (it->type_ == ParserItem::Type::Token && it->token_.token_type_ == TokenType::KleeneStar)
        {
            handleKleene(it);
            it = items_.erase(it);
        }

        else if (it->type_ == ParserItem::Type::Token && it->token_.token_type_ == TokenType::Question)
        {
            handleQuestion(it); //create or-node(r|эпсилон)
            it = items_.erase(it);
        }

        else if (it->type_ == ParserItem::Type::Token && it->token_.token_type_ == TokenType::Repeat)
        {
            handleRepeat(it); //create or-node(r|эпсилон)
            it = items_.erase(it);
        }

        else { it++; }

    }
};

void Parser::handleRepeat(auto& it)
{
    size_t count = it->token_.repeat_value.value();

    if (count == 0)
    {
        *std::prev(it) = ParserItem(std::make_unique<ANode>());
    }

    else if (count == 1)
    {
        return;
    }

    else if (count > 1)
    {
        auto origin_node = std::move(std::prev(it)->node_);
        auto result_node = origin_node->clone();

        for (size_t i = 2; i <= count; i++)
        {
            auto clone_node = origin_node->clone();
            result_node = std::make_unique<ConcatNode>(std::move(result_node), std::move(clone_node));
        }

        *std::prev(it) = ParserItem(std::move(result_node));
    }
};

void Parser::handleQuestion(auto& it)
{
    if (std::prev(it)->type_ != ParserItem::Type::Node)
    {
        throw std::runtime_error("Синтаксическая ошибка при написании ОПЦИОНАЛЬНАЯ ЧАСТЬ");
    }

    auto or_node = std::make_unique<OrNode>(std::move(std::prev(it)->node_), std::make_unique<ANode>());
    *std::prev(it) = ParserItem(std::move(or_node));
};

void Parser::handleKleene(auto& it)
{
    if (std::prev(it)->type_ != ParserItem::Type::Node)
    {
        throw std::runtime_error("Синтаксическая ошибка при написании ЗАМЫКАНИЕ КЛИНИ");
    }

    auto star = std::make_unique<StarNode>(std::move(std::prev(it)->node_));
    *std::prev(it) = ParserItem(std::move(star));
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

void Parser::saveNamedGroup(std::list<ParserItem>::iterator start, std::list<ParserItem>::iterator last)
{
    if (start->type_ == ParserItem::Type::Token && start->token_.token_type_ == TokenType::NamedGroupName)
    {
        std::string name = start->token_.group_name.value();

        auto& inner_node = std::next(start)->node_; 
       std::cout << "[PARSER] SAVING GROUP TO MAP: '" << name << "'\n";
        captured_groups_[name] = inner_node->clone();
    }
}
