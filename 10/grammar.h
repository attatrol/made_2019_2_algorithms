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
    { Token::STMT,    { Token::LET, Token::VARIABLE, Token::R_ASSIGN } }, // 0
    { Token::STMT,    { Token::DEF, Token::FN_DEF, Token::R_ASSIGN } }, // 1
    { Token::FN_DEF,   { Token::VARIABLE, Token::LEFT_BRACKET, Token::FN_ARGS, Token::RIGHT_BRACKET } }, // 2
    { Token::FN_ARGS, { Token::VARIABLE, Token::FN_ARGS } }, // 3
    { Token::FN_ARGS, { Token::COMMA, Token::VARIABLE, Token::FN_ARGS } }, // 4
    { Token::FN_ARGS, { Token::NONE } }, // 5
    { Token::STMT,    { Token::EXPR, Token::R_ASSIGN } }, // 6
    { Token::R_ASSIGN,   { Token::EQ, Token::EXPR } }, // 7
    { Token::R_ASSIGN,   { Token::NONE} }, // 8
    { Token::EXPR,    { Token::SUM_L, Token::SUM_R } }, // 9
    { Token::SUM_R,   { Token::PLUS, Token::EXPR } }, // 10
    { Token::SUM_R,   { Token::MINUS, Token::EXPR } }, // 11
    { Token::SUM_R,   { Token::NONE } }, // 12
    { Token::SUM_L,    { Token::PROD_L, Token::PROD_R } }, // 13
    { Token::PROD_R,   { Token::MUL_SIGN, Token::SUM_L } }, // 14
    { Token::PROD_R,   { Token::SLASH, Token::SUM_L } }, // 15
    { Token::PROD_R,   { Token::NONE } }, // 16
    { Token::PROD_L,    { Token::PLUS, Token::PROD_L } }, // 17
    { Token::PROD_L,    { Token::MINUS, Token::PROD_L } }, // 18
    { Token::PROD_L,    { Token::TERM } }, // 19
    { Token::TERM,   { Token::RE, Token::LEFT_BRACKET, Token::EXPR, Token::RIGHT_BRACKET } }, // 20
    { Token::TERM,   { Token::IM, Token::LEFT_BRACKET, Token::EXPR, Token::RIGHT_BRACKET } }, // 20
    { Token::TERM,   { Token::VARIABLE, Token::CALL } }, // 20
    { Token::TERM,   { Token::NUMBER, Token::DOT_PART } }, // 21
    { Token::DOT_PART,   { Token::POINT, Token::NUMBER } }, // 22
    { Token::DOT_PART,   { Token::NONE } }, // 23
    { Token::CALL, { Token::LEFT_BRACKET, Token::CRGS, Token::RIGHT_BRACKET } }, // 24
    { Token::CALL, { Token::NONE } }, // 25
    { Token::CRGS, { Token::EXPR, Token::CRGS2 } }, // 26
    { Token::CRGS, { Token::NONE } }, // 27
    { Token::CRGS2,{ Token::COMMA, Token::CRGS } }, // 28
    { Token::CRGS2,{ Token::NONE } }, // 29
    { Token::TERM,   { Token::LEFT_SQR_BRACKET, Token::EXPR, Token::SEMICOLON, Token::EXPR, Token::RIGHT_SQR_BRACKET } }, // 30
    { Token::TERM,   { Token::LEFT_BRACKET, Token::EXPR, Token::RIGHT_BRACKET } }, // 31
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
