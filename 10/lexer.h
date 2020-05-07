#ifndef LEXER_H
#define LEXER_H

#include "lexeme.h"

#include <vector>

class LexemeMatcher
{
private:
    const Token type_;
    const int priority_;
public:
    LexemeMatcher(Token type, int priority) : type_(type), priority_(priority)
    {
    }
    virtual ~LexemeMatcher()
    {
    }
    virtual bool find(const std::string& input, const std::size_t start, std::size_t& end) const = 0;
    Token getLexemeType() const
    {
        return type_;
    }
    int getPriority() const
    {
        return priority_;
    }
};

class WhitespaceLexemeMatcher: public LexemeMatcher
{
public:
    WhitespaceLexemeMatcher() : LexemeMatcher(Token::SPACE, SPACE_LEXEME_PRIORITY)
    {
    }
    bool find(const std::string &input, const std::size_t start, std::size_t &end) const override;
};

class CharLexemeMatcher: public LexemeMatcher
{
private:
    const char char_;
public:
    CharLexemeMatcher(Token type, char c) : LexemeMatcher(type, SINGLE_SYMBOL_LEXEME_PRIORITY), char_(c)
    {
    }
    bool find(const std::string &input, const std::size_t start, std::size_t &end) const override;
};

class IdLexemeMatcher: public LexemeMatcher
{
public:
    IdLexemeMatcher() : LexemeMatcher(Token::VARIABLE, ID_LEXEME_PRIORITY)
    {
    }
    bool find(const std::string &input, const std::size_t start, std::size_t &end) const override;
};

class NumberLexemeMatcher: public LexemeMatcher
{
public:
    NumberLexemeMatcher() : LexemeMatcher(Token::NUMBER, NUMBER_LEXEME_PRIORITY)
    {
    }
    bool find(const std::string &input, const std::size_t start, std::size_t &end) const override;
};

static const LexemeMatcher* MATCHERS[] =
{
    new WhitespaceLexemeMatcher(),
    new CharLexemeMatcher(Token::POINT, '.'),
    new CharLexemeMatcher(Token::COMMA, ','),
    new CharLexemeMatcher(Token::SEMICOLON, ';'),
    new CharLexemeMatcher(Token::LT, '<'),
    new CharLexemeMatcher(Token::GT, '>'),
    new CharLexemeMatcher(Token::EQ, '='),
    new CharLexemeMatcher(Token::PLUS, '+'),
    new CharLexemeMatcher(Token::MINUS, '-'),
    new CharLexemeMatcher(Token::MUL_SIGN, '*'),
    new CharLexemeMatcher(Token::SLASH, '/'),
    new CharLexemeMatcher(Token::LEFT_BRACKET, '('),
    new CharLexemeMatcher(Token::RIGHT_BRACKET, ')'),
    new IdLexemeMatcher(),
    new NumberLexemeMatcher(),
};

struct LexerException : public std::exception
{
    const std::size_t pos_;

    LexerException(std::size_t pos): pos_(pos)
    {
    }

    const char* what() const noexcept override
    {
        return "Lexer failed to extract a lexeme";
    }
};

std::vector<Lexeme> getLexemes(const std::string& input);

#endif // LEXER_H
