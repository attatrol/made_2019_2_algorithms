#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "grammar.h"
#include "lexeme.h"
#include "lexer.h"
#include "parser.h"

struct InterpreterValue
{
    bool defined_;
    float numValue_;
    bool boolValue_;

    InterpreterValue(): defined_(false)
    {
    }
};

class Interpreter
{
public:
    Interpreter(const std::vector<GrammarRule>& grammar);


    auto ast = parse(GRAMMAR, table, lexemes);
};

#endif // INTERPRETER_H
