#include <assert.h>
#include "lexer.h"

std::vector<Lexeme> getLexemes(const std::string &input)
{
    std::vector<Lexeme> lexemes;

    const std::size_t size = input.size();
    std::size_t start = 0, end;

    while (start < size)
    {
        bool matched = false;
        for (const LexemeMatcher* matcher : MATCHERS)
        {
            if (matcher->find(input, start, end))
            {
                lexemes.emplace_back(matcher->getLexemeType(), start, end);
                matched = true;
                start = end + 1;
                break;
            }
        }
        if (!matched)
        {
            throw LexerException(start);
        }
    }

    lexemes.emplace_back(Token::EOP, size, size);

    // prune spaces
    // parser doesn't know about spaces for the sake of simplicity
    std::vector<Lexeme> result;
    for (const Lexeme& lexeme : lexemes)
    {
        if (lexeme.type_ != Token::SPACE)
        {
            assert(isTerminal(lexeme.type_));
            result.emplace_back(lexeme);
        }
    }


    return lexemes;
}
