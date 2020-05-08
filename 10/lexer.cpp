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
    // parser doesn't know about spaces for the sake of the grammar simplicity
    std::vector<Lexeme> result;
    for (const Lexeme& lexeme : lexemes)
    {
        if (lexeme.type_ != Token::SPACE)
        {
            assert(isTerminal(lexeme.type_));
            result.emplace_back(lexeme);
        }
    }
    return result;
}

bool WhitespaceLexemeMatcher::find(const std::string &input, const std::size_t start, std::size_t &end) const
{
    if (isspace(input[start]))
    {
        end = start;
        while (isspace(input[end + 1]))
        {
            ++end;
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool CharLexemeMatcher::find(const std::string &input, const std::size_t start, std::size_t &end) const
{
    if(input[start] == char_)
    {
        end = start;
        return true;
    }
    else
    {
        return false;
    }
}

bool SpecificIdLexemeMatcher::find(const std::string &input, const std::size_t start, std::size_t &end) const
{
    const std::size_t size = literal_.size();
    if (input.size() - start < size)
    {
        return false;
    }
    for (std::size_t i = 0; i < size; ++i)
    {
        if (literal_[i] != input[start + i])
        {
            return false;
        }
    }
    end = start + size - 1;
    return true;
}

bool IdLexemeMatcher::find(const std::string &input, const std::size_t start, std::size_t &end) const
{
    if(isalpha(input[start]))
    {
        end = start;
        while (isalnum(input[end + 1]))
        {
            ++end;
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool NumberLexemeMatcher::find(const std::string &input, const std::size_t start, std::size_t &end) const
{
    if(isdigit(input[start]))
    {
        end = start;
        while (isdigit(input[end + 1]))
        {
            ++end;
        }
        return true;
    }
    else
    {
        return false;
    }
}
