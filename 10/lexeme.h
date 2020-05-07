#ifndef LEXEME_H
#define LEXEME_H

#include <string>

/**
  * Token types
  */
enum Token
{
    NONE = 0,
    // empty space
    SPACE = 1,
    // single symbol lexemes
    POINT = 2,         // .
    COMMA = 3,         // ,
    SEMICOLON = 4,     // ;
    LT = 5,            // <
    GT = 6,            // >
    EQ = 7,            // =
    PLUS = 8,          // +
    MINUS = 9,         // -
    MUL_SIGN = 10,      // *
    SLASH = 11,         // /
    LEFT_BRACKET = 12,  // (
    RIGHT_BRACKET = 13, // )

    // other lexemes
    ID = 100,    // legal identifier, starts with a letter followed by alphanumeric sequence
    NUMBER = 101, // number sequence

    // end-of-parce
    EOP = 999,

    // non-terminals
    E =  1001,
    T =  1002,
    E1 = 1003,
    F =  1004,
    T1 = 1005,

    // test set
    S = 2000,
    A = 2001,
    B = 2002,
    C = 2003,
    a = 50,
    b = 51,
    g = 52,
    h = 53,
    d = 54,
};

inline bool isTerminal(Token token)
{
    return token < 1000;
}

struct Lexeme
{
    Token type_;
    std::size_t start_;
    std::size_t end_;

    Lexeme(Token type, std::size_t start, std::size_t end) : type_(type), start_(start), end_(end)
    {
    }
    Lexeme(const Lexeme& other) : type_(other.type_), start_(other.start_), end_(other.end_)
    {
    }
};

static const int SPACE_LEXEME_PRIORITY = 3;
static const int SINGLE_SYMBOL_LEXEME_PRIORITY = 2;
static const int ID_LEXEME_PRIORITY = 1;
static const int NUMBER_LEXEME_PRIORITY = 0;

#endif // LEXEME_H
