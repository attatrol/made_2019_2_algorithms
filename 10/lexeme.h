#ifndef LEXEME_H
#define LEXEME_H

#include <string>

/**
  * Token types
  */
enum Token
{
    // epsilon
    NONE = 0,
    // empty space
    SPACE = 1,
    // single symbol lexemes
    POINT = 2,          // .
    COMMA = 3,          // ,
    SEMICOLON = 4,      // ;
    LT = 5,             // <
    GT = 6,             // >
    EQ = 7,             // =
    PLUS = 8,           // +
    MINUS = 9,          // -
    MUL_SIGN = 10,      // *
    SLASH = 11,         // /
    LEFT_BRACKET = 12,  // (
    RIGHT_BRACKET = 13, // )
    LEFT_SQR_BRACKET = 14,  // [
    RIGHT_SQR_BRACKET = 15, // ]

    // other lexemes
    LET = 50,       // variable declaration
    DEF = 51,       // function declaration
    RE  = 52,       // real part of the number
    IM  = 53,       // imaginary part of the number
    VARIABLE = 100, // legal identifier, starts with a letter followed by alphanumeric sequence
    NUMBER = 101,   // number sequence

    // end-of-parce
    EOP = 999,

    // non-terminals
    S    = 1000,
    S1   = 1001,
    S2   = 1002,
    FN   = 1003,
    ARGS = 1004,
    E    = 1020,
    T    = 1030,
    E1   = 1040,
    F    = 1050,
    T1   = 1060,
    F1   = 1070,
    F2   = 1080,
    CALL = 1090,
    CRGS = 1100, // call args
    CRGS2= 1200,
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
