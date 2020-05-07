#include "grammar.h"

std::pair<std::unordered_set<Token>, std::unordered_set<Token>> getTerminalsAndNonTerminals(const std::vector<GrammarRule> &grammar)
{
    std::unordered_set<Token> nonTerminals;
    for (auto& rule : grammar)
    {
        nonTerminals.insert(rule.lhs_);
    }

    std::unordered_set<Token> terminals;
    for (auto& rule : grammar)
    {
        for (Token token : rule.rhs_)
        {
            if (!nonTerminals.count(token))
            {
                terminals.insert(token);
            }
        }
    }
    terminals.insert(Token::EOP);

    return std::make_pair(terminals, nonTerminals);
}


std::unordered_map<Token, std::unordered_set<Token>> getFirsts(const std::vector<GrammarRule>& grammar,
                                                               const std::unordered_set<Token>& nonTerminals)
{

    std::unordered_map<Token, std::unordered_set<Token>> result;
    for (auto nonTerminal : nonTerminals)
    {
        result[nonTerminal] = std::unordered_set<Token>();
    }

    bool changed = true;
    while (changed)
    {
        changed = false;
        for (auto& rule : grammar)
        {
            Token nonTerminal = rule.lhs_;
            std::size_t index = 0;
            Token firstToken = rule.rhs_[0];
            while (true)
            {
                bool next = false;
                if (nonTerminals.count(firstToken))
                {
                    assert(!isTerminal(firstToken));
                    for (auto firstTokenFirst : result[firstToken])
                    {
                        if (firstTokenFirst == Token::NONE)
                        {
                            next = true;
                        }
                        else
                        {
                            assert(isTerminal(firstTokenFirst));
                            if (!result[nonTerminal].count(firstTokenFirst))
                            {
                                changed = true;
                                result[nonTerminal].insert(firstTokenFirst);
                            }
                        }
                    }
                }
                else
                {
                    assert(isTerminal(firstToken));
                    if (!result[nonTerminal].count(firstToken))
                    {
                        changed = true;
                        result[nonTerminal].insert(firstToken);
                    }
                }
                if (next)
                {
                    ++index;
                    if (index < rule.rhs_.size())
                    {
                        firstToken = rule.rhs_[index];
                    }
                    else
                    {
                        if (!result[nonTerminal].count(Token::NONE))
                        {
                            changed = true;
                            result[nonTerminal].insert(Token::NONE);
                        }
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
        }
    }

    return result;
}

std::unordered_map<Token, std::unordered_set<Token>> getFollow(const std::vector<GrammarRule> &grammar,
                                                               const std::unordered_set<Token>& nonTerminals,
                                                               const std::unordered_map<Token, std::unordered_set<Token>>& firsts)
{
    std::unordered_map<Token, std::unordered_set<Token>> result;
    for (auto nonTerminal : nonTerminals)
    {
        result[nonTerminal] = std::unordered_set<Token>();
    }
    result[grammar[0].lhs_].insert(Token::EOP);

    bool changed = true;
    while (changed)
    {
        changed = false;
        for (auto& rule : grammar)
        {
            for (std::size_t i = 0, size = rule.rhs_.size(); i < size; ++i)
            {
                if (nonTerminals.count(rule.rhs_[i]))
                {
                    Token currentNonTerminal = rule.rhs_[i];
                    for (std::size_t j = i + 1; j < size; ++j)
                    {
                        if (nonTerminals.count(rule.rhs_[j]))
                        {
                            for (Token token : firsts.at(rule.rhs_[j]))
                            {
                                if (token == Token::NONE)
                                {
                                    for (Token token2 : result.at(rule.lhs_))
                                    {
                                        if (!result[currentNonTerminal].count(token2))
                                        {
                                            result[currentNonTerminal].insert(token2);
                                            changed = true;
                                        }
                                    }
                                }
                                else if (!result[currentNonTerminal].count(token))
                                {
                                    result[currentNonTerminal].insert(token);
                                    changed = true;
                                }
                            }
                        }
                        else if (!result[currentNonTerminal].count(rule.rhs_[j]))
                        {
                            result[currentNonTerminal].insert(rule.rhs_[j]);
                            changed = true;
                        }
                    }
                    if (i == size - 1)
                    {
                        for (Token token : result.at(rule.lhs_))
                        {
                            if (!result[currentNonTerminal].count(token))
                            {
                                result[currentNonTerminal].insert(token);
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    }

    return result;
}

std::unordered_map<Token, std::unordered_map<Token, ParserTableEntry>> buildTable(const std::vector<GrammarRule> &grammar,
                                                                                  const std::unordered_set<Token>& terminals,
                                                                                  const std::unordered_set<Token>& nonTerminals,
                                                                                  const std::unordered_map<Token, std::unordered_set<Token>>& firsts,
                                                                                  const std::unordered_map<Token, std::unordered_set<Token>>& follow)
{
    std::unordered_map<Token, std::unordered_map<Token, ParserTableEntry>> result;
    for (Token nonTerminal : nonTerminals)
    {
        result[nonTerminal] = std::unordered_map<Token, ParserTableEntry>();
        for (Token terminal : terminals)
        {
            result[nonTerminal][terminal] = { false, 0 };
        }
    }

    std::vector<std::unordered_set<Token>> firstsInRules(grammar.size());

    for (std::size_t i = 0, size = grammar.size(); i < size; ++i)
    {
        const GrammarRule& rule = grammar[i];
        for (Token token : rule.rhs_)
        {
            bool gotoNext = false;
            if (firsts.count(token))
            {
                for (Token token2 : firsts.at(token))
                {
                    if (token2 == Token::NONE)
                    {
                        gotoNext = true;
                    }
                    firstsInRules[i].insert(token2);
                }
            }
            else
            {
                firstsInRules[i].insert(token);
            }
            if (!gotoNext)
            {
                break;
            }
        }
    }

    for (std::size_t i = 0, size = grammar.size(); i < size; ++i)
    {
        const GrammarRule& rule = grammar[i];
        const Token nonTerminal = rule.lhs_;
        const std::unordered_set<Token>& firstsInRule = firstsInRules[i];
        for (Token terminal : terminals)
        {
            if (firstsInRule.count(terminal))
            {
                if (result[nonTerminal][terminal].validEntry_ && result[nonTerminal][terminal].ruleIndex_ != i)
                {
                    throw new NonLL1GrammarException;
                }
                result[nonTerminal][terminal].validEntry_ = true;
                result[nonTerminal][terminal].ruleIndex_ = i;
            }
        }
        if (firstsInRule.count(Token::NONE))
        {
            const std::unordered_set<Token>& followInRule = follow.at(nonTerminal);
            for (Token terminal : terminals)
            {
                if (followInRule.count(terminal))
                {
                    if (result[nonTerminal][terminal].validEntry_ && result[nonTerminal][terminal].ruleIndex_ != i)
                    {
                        throw new NonLL1GrammarException;
                    }
                    result[nonTerminal][terminal].validEntry_ = true;
                    result[nonTerminal][terminal].ruleIndex_ = i;
                }
            }
        }
    }

    return result;
}
std::unordered_map<Token, std::unordered_map<Token, ParserTableEntry>> buildTable(const std::vector<GrammarRule> &grammar)
{
    auto [terminals, nonTerminals] = getTerminalsAndNonTerminals(grammar);
            auto firsts = getFirsts(grammar, nonTerminals);
            auto follow = getFollow(grammar, nonTerminals, firsts);
            return buildTable(grammar, terminals, nonTerminals, firsts, follow);
}
