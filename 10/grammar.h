#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <assert.h>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "lexeme.h"

struct ValuedTerminal
{
    Token lexemeType_;
    std::string value_;
};

static const ValuedTerminal ID_TERM = { Token::VARIABLE, "" };
static const ValuedTerminal NUMBER_TERM = { Token::NUMBER, "" };
static const ValuedTerminal LET_TERM = { Token::VARIABLE, "let" };

struct NonTerminal
{
    std::size_t index_;
    std::string name_; // to be deleted, only for debug purposes
};

struct GrammarRule
{
    Token lhs_;
    std::vector<Token> rhs_;
};

// >= 1000 - non-terminals
// >= 100  - terminals with value
// >  0    - single letter terminals
// 0       - epsilon

static const std::vector<GrammarRule> GRAMMAR = {
    { Token::S,  { Token::E, Token::S1 } }, // S -> E S1
    { Token::S1, { Token::EQ, Token::E } }, // S1 -> =E
    { Token::S1, { Token::NONE} }, // S1 -> epsilon
    { Token::E,  { Token::T, Token::E1 } }, // E -> TE'
    { Token::E1, { Token::PLUS, Token::E } }, // E' -> +E
    { Token::E1, { Token::MINUS, Token::E } }, // E' -> -E
    { Token::E1, { Token::NONE } }, // E' -> epsilon
    { Token::T,  { Token::F, Token::T1 } }, // T -> FT'
    { Token::T1, { Token::MUL_SIGN, Token::T } }, // T' -> *T
    { Token::T1, { Token::SLASH, Token::T } }, // T' -> /T
    { Token::T1, { Token::NONE } }, // T' -> epsilon
    { Token::F,  { Token::VARIABLE } }, // F -> id
    { Token::F,  { Token::NUMBER } }, // F -> number
    { Token::F,  { Token::LEFT_BRACKET, Token::E, Token::RIGHT_BRACKET } }, // F -> (E)
};

//static const std::vector<GrammarRule> TEST_GRAMMAR = {
//    { Token::S, { Token::A, Token::B, Token::C } },
//    { Token::S, { Token::C, Token::b, Token::b } },
//    { Token::S, { Token::B, Token::a } },
//    { Token::A, { Token::d, Token::a } },
//    { Token::A, { Token::B, Token::C } },
//    { Token::B, { Token::g } },
//    { Token::B, { Token::NONE } },
//    { Token::C, { Token::h } },
//    { Token::C, { Token::NONE } },
//};

struct ParserTableEntry
{
    bool validEntry_;
    std::size_t ruleIndex_;
};

struct NonLL1GrammarException : public std::exception
{

    const char* what() const noexcept override
    {
        return "Grammar is not a LL(1) grammar";
    }
};

std::pair<std::unordered_set<Token>, std::unordered_set<Token>> getTerminalsAndNonTerminals(const std::vector<GrammarRule>& grammar);

std::unordered_map<Token, std::unordered_set<Token>> getFirsts(const std::vector<GrammarRule>& grammar,
                                                               const std::unordered_set<Token>& nonTerminals);

std::unordered_map<Token, std::unordered_set<Token>> getFollow(const std::vector<GrammarRule>& grammar,
                                                               const std::unordered_set<Token>& nonTerminals,
                                                               const std::unordered_map<Token, std::unordered_set<Token>>& firsts);

std::unordered_map<Token, std::unordered_map<Token, ParserTableEntry>> buildTable(const std::vector<GrammarRule>& grammar,
                                                                                  const std::unordered_set<Token>& terminals,
                                                                                  const std::unordered_set<Token>& nonTerminals,
                                                                                  const std::unordered_map<Token, std::unordered_set<Token>>& firsts,
                                                                                  const std::unordered_map<Token, std::unordered_set<Token>>& follow);

std::unordered_map<Token, std::unordered_map<Token, ParserTableEntry>> buildTable(const std::vector<GrammarRule>& grammar);

#endif // GRAMMAR_H
