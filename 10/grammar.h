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
    { Token::S,    { Token::LET, Token::VARIABLE, Token::S1 } }, // 0
    { Token::S,    { Token::DEF, Token::FN, Token::S1 } }, // 1
    { Token::FN,   { Token::VARIABLE, Token::LEFT_BRACKET, Token::ARGS, Token::RIGHT_BRACKET } }, // 2
    { Token::ARGS, { Token::VARIABLE, Token::ARGS } }, // 3
    { Token::ARGS, { Token::COMMA, Token::VARIABLE, Token::ARGS } }, // 4
    { Token::ARGS, { Token::NONE } }, // 5
    { Token::S,    { Token::E, Token::S1 } }, // 6
    { Token::S1,   { Token::EQ, Token::E } }, // 7
    { Token::S1,   { Token::NONE} }, // 8
    { Token::E,    { Token::T, Token::E1 } }, // 9
    { Token::E1,   { Token::PLUS, Token::E } }, // 10
    { Token::E1,   { Token::MINUS, Token::E } }, // 11
    { Token::E1,   { Token::NONE } }, // 12
    { Token::T,    { Token::F, Token::T1 } }, // 13
    { Token::T1,   { Token::MUL_SIGN, Token::T } }, // 14
    { Token::T1,   { Token::SLASH, Token::T } }, // 15
    { Token::T1,   { Token::NONE } }, // 16
    { Token::F,    { Token::PLUS, Token::F } }, // 17
    { Token::F,    { Token::MINUS, Token::F } }, // 18
    { Token::F,    { Token::F1 } }, // 19
    { Token::F1,   { Token::RE, Token::LEFT_BRACKET, Token::E, Token::RIGHT_BRACKET } }, // 20
    { Token::F1,   { Token::IM, Token::LEFT_BRACKET, Token::E, Token::RIGHT_BRACKET } }, // 20
    { Token::F1,   { Token::VARIABLE, Token::CALL } }, // 20
    { Token::F1,   { Token::NUMBER, Token::F2 } }, // 21
    { Token::F2,   { Token::POINT, Token::NUMBER } }, // 22
    { Token::F2,   { Token::NONE } }, // 23
    { Token::CALL, { Token::LEFT_BRACKET, Token::CRGS, Token::RIGHT_BRACKET } }, // 24
    { Token::CALL, { Token::NONE } }, // 25
    { Token::CRGS, { Token::E, Token::CRGS2 } }, // 26
    { Token::CRGS, { Token::NONE } }, // 27
    { Token::CRGS2,{ Token::COMMA, Token::CRGS } }, // 28
    { Token::CRGS2,{ Token::NONE } }, // 29
    { Token::F1,   { Token::LEFT_SQR_BRACKET, Token::E, Token::SEMICOLON, Token::E, Token::RIGHT_SQR_BRACKET } }, // 30
    { Token::F1,   { Token::LEFT_BRACKET, Token::E, Token::RIGHT_BRACKET } }, // 31
};

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
