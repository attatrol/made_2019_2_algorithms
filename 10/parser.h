#ifndef PARSER_H
#define PARSER_H

#include <stack>

#include "grammar.h"
#include "lexeme.h"

struct ASTNode
{
    Lexeme lexeme_;
    ASTNode* parent_;
    std::vector<ASTNode*> children_;

    ASTNode(Token token, ASTNode* parent) : lexeme_(token, 1, 0), parent_(parent)
    {
    }
    ASTNode(const Lexeme& lexeme, ASTNode* parent) : lexeme_(lexeme.type_, lexeme.start_, lexeme.end_), parent_(parent)
    {
    }
    ASTNode(const ASTNode& other) : lexeme_(other.lexeme_.type_, other.lexeme_.start_, other.lexeme_.end_), parent_(nullptr)
    {
        for (const ASTNode* child : other.children_)
        {
            children_.push_back(new ASTNode(*child));
        }
        for (ASTNode* child : children_)
        {
            child->parent_ = this;
        }
    }
    ~ASTNode()
    {
        for (ASTNode* child : children_)
        {
            delete child;
        }
    }
};

struct ParserException : public std::exception
{
    const std::size_t pos_;

    ParserException(std::size_t pos): pos_(pos)
    {
    }

    const char* what() const noexcept override
    {
        return "Parser failed to build an AST";
    }
};

ASTNode* parse(const std::vector<GrammarRule>& grammar,
              const std::unordered_map<Token, std::unordered_map<Token, ParserTableEntry>>& table,
              const std::vector<Lexeme>& lexemes);

#endif // PARSER_H
