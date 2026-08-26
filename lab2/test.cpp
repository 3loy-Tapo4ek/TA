#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "CoolRegex.hpp"
#include "FAOperator.hpp"
#include "Tokenizer.hpp"
#include "Parser.hpp"
#include "NFABuilder.hpp"
#include "DFABuilder.hpp"

// ============================================================================
// 1. АНАЛИЗ И СИНТАКСИС (AST & LEXER)
// ============================================================================

TEST_CASE("AST & Parser: Синтаксический анализ и обработка метасимволов", "[ast]") {
    Tokenizer tokenizer;
    Parser parser;

    SECTION("Корректный парсинг всех поддерживаемых операторов") {
        REQUIRE_NOTHROW(parser.Parse(tokenizer.tokenize("a|b")));
        REQUIRE_NOTHROW(parser.Parse(tokenizer.tokenize("a...b?c{3}")));
        REQUIRE_NOTHROW(parser.Parse(tokenizer.tokenize("%s%|%|%"))); // Экранирование
        
        auto ast = parser.Parse(tokenizer.tokenize("a"));
        REQUIRE(ast != nullptr);
    }

    SECTION("Именованные группы захвата (<name>r) и ссылки <name>") {
        REQUIRE_NOTHROW(parser.Parse(tokenizer.tokenize("(<group1>a|b)c<group1>")));
    }
}

// ============================================================================
// 2. НКА (NFA & THOMPSON CONSTRUCTION)
// ============================================================================

TEST_CASE("NFA: Построение НКА по алгоритму Томпсона", "[nfa]") {
    Tokenizer tokenizer;
    Parser parser;
    NFABuilder nfa_builder;

    SECTION("Создание НКА из AST") {
        auto ast = parser.Parse(tokenizer.tokenize("a|b..."));
        FA nfa = nfa_builder.buildNFA(ast);

        // Граф Томпсона должен иметь корректные индексы состояний
        REQUIRE(nfa.states_.size() >= 4);
        REQUIRE(nfa.start_ptr_ < nfa.states_.size());
    }
}

// ============================================================================
// 3. ДКА И ИЗОМОРФИЗМ (DFA & ISOMORPHISM)
// ============================================================================

TEST_CASE("DFA: Детерминизация, Минимизация и Изоморфизм", "[dfa][isomorphism]") {
    SECTION("Изоморфизм коммутативных выражений (a|b) и (b|a)") {
        auto re1 = CoolRegex::compile("a|b");
        auto re2 = CoolRegex::compile("b|a");

        // Минимальные ДКА для эквивалентных языков ДОЛЖНЫ быть изоморфны!
        REQUIRE(FAOperator::isIsomorphic(re1.getDFA(), re2.getDFA()));
    }

    SECTION("Изоморфизм эквивалентных квантификаторов (a{2} и aa)") {
        auto re1 = CoolRegex::compile("a{2}");
        auto re2 = CoolRegex::compile("aa");

        REQUIRE(FAOperator::isIsomorphic(re1.getDFA(), re2.getDFA()));
    }
}

// ============================================================================
// 4. ТЕОРЕТИКО-МНОЖЕСТВЕННЫЕ ОПЕРАЦИИ (COMPLEMENT & DIFFERENCE)
// ============================================================================

TEST_CASE("DFA: Операции над языками (Дополнение и Разность)", "[dfa][math]") {
    SECTION("Дополнение языка (Complement)") {
        auto re_a = CoolRegex::compile("a");
        CoolRegex comp_a = re_a.complement();

        // Дополнение языка {"a"} не должно принимать "a", но должно принимать "b"
        REQUIRE(comp_a.findAll("a").empty());
        REQUIRE_FALSE(comp_a.findAll("aa").empty());
    }

    SECTION("Разность языков (L1 \\ L2)") {
        auto l1 = CoolRegex::compile("a|b|c");
        auto l2 = CoolRegex::compile("b");

        FA diff_fa = FAOperator::MakeDifference(l1.getDFA(), l2.getDFA());
        CoolRegex diff_re(std::move(diff_fa));

        REQUIRE_FALSE(diff_re.findAll("a").empty());
        REQUIRE_FALSE(diff_re.findAll("c").empty());
        
        // Символ 'b' входит и в L1, и в L2 -> вычитается!
        REQUIRE(diff_re.findAll("b").empty());
    }

    SECTION("Разность одинаковых языков (L - L = Ø)") {
        auto l1 = CoolRegex::compile("a|b|c");
        FA diff_fa = FAOperator::MakeDifference(l1.getDFA(), l1.getDFA());
        CoolRegex diff_re(std::move(diff_fa));

        REQUIRE(diff_re.findAll("a b c").empty());
    }
}

// ============================================================================
// 5. ВОССТАНОВЛЕНИЕ K-ПУТИ (MCNAUGHTON-YAMADA)
// ============================================================================

TEST_CASE("DFA: Восстановление регулярного выражения методом K-пути", "[dfa][k-path]") {
    SECTION("Восстановление базовых выражений") {
        auto orig = CoolRegex::compile("a|b");
        std::string regen_str = FAOperator::ReconstructRegex(orig.getDFA());

        auto restored = CoolRegex::compile(regen_str);

        // Исходный и восстановленный автоматы изоморфны!
        REQUIRE(FAOperator::isIsomorphic(orig.getDFA(), restored.getDFA()));
    }

    SECTION("Восстановление сложных выражений с Замыканием Клини") {
        auto orig = CoolRegex::compile("(a{2}|b)...");
        std::string regen_str = FAOperator::ReconstructRegex(orig.getDFA());

        auto restored = CoolRegex::compile(regen_str);

        // Автомат, восстановленный из K-пути, эквивалентен исходному
        REQUIRE(FAOperator::isIsomorphic(orig.getDFA(), restored.getDFA()));
    }
}

// ============================================================================
// 6. ПОИСК В ТЕКСТЕ (FINDALL & GREEDY MATCHING)
// ============================================================================

TEST_CASE("CoolRegex: Поиск совпадений в тексте (findAll)", "[findall]") {
    SECTION("Непересекающиеся вхождения") {
        auto re = CoolRegex::compile("101");
        auto matches = re.findAll("10101");

        // "10101" содержит два "101", но они пересекаются.
        // Забирается только первое "101".
        REQUIRE(matches.size() == 1);
        REQUIRE(matches[0] == "101");
    }

    SECTION("Опциональность '?'") {
        auto re = CoolRegex::compile("a(bc)?d");
        auto matches = re.findAll("abcd ad");

        REQUIRE(matches.size() == 2);
        REQUIRE(matches[0] == "abcd");
        REQUIRE(matches[1] == "ad");
    }

    SECTION("Фиксированный повтор '{x}'") {
        auto re = CoolRegex::compile("a{3}");
        auto matches = re.findAll("aa aaa aaaa");

        REQUIRE(matches.size() == 2); // "aaa" и первичное "aaa" из "aaaa"
        REQUIRE(matches[0] == "aaa");
        REQUIRE(matches[1] == "aaa");
    }
}