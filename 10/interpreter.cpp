#include "interpreter.h"

Interpreter::Interpreter(const std::vector<GrammarRule>& grammar)
{
    auto lexemes = getLexemes(input);
    auto firsts = getFirsts(GRAMMAR);
    auto follow = getFollow(GRAMMAR, firsts);
    auto table = buildTable(GRAMMAR, firsts, follow);

    auto ast = parse(GRAMMAR, table, lexemes);
}
