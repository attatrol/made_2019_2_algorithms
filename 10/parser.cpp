#include "parser.h"


ASTNode* parse(const std::vector<GrammarRule>& grammar,
               const std::unordered_map<Token, std::unordered_map<Token, ParserTableEntry>>& table,
               const std::vector<Lexeme>& lexemes)
{
    ASTNode* root = new ASTNode(grammar[0].lhs_, nullptr);

    std::stack<ASTNode*> stack;
    stack.push(root);
    std::size_t lexemeIndex = 0;
    while (!stack.empty())
    {
        ASTNode* node = stack.top();
        const Token nodeType = node->lexeme_.type_;
        const Token lexemeType = lexemes[lexemeIndex].type_;
        stack.pop();
        if (nodeType == Token::NONE)
        {
            continue;
        }
        if (isTerminal(nodeType))
        {
            if(lexemeType != nodeType)
            {
                while (node->parent_)
                {
                    node = node->parent_;
                }
                delete node;
                throw ParserException(lexemeIndex);
            }
            node->lexeme_.start_ = lexemes[lexemeIndex].start_;
            node->lexeme_.end_ = lexemes[lexemeIndex].end_;
            ++lexemeIndex;
            continue;
        }
        const ParserTableEntry& entry = table.at(nodeType).at(lexemeType);
        if (!entry.validEntry_)
        {
            while (node->parent_)
            {
                node = node->parent_;
            }
            delete node;
            throw ParserException(lexemeIndex);
        }
        const GrammarRule& rule = grammar[entry.ruleIndex_];
        assert(rule.lhs_ == nodeType);
        for (Token token : rule.rhs_)
        {
            node->children_.push_back(new ASTNode(token, node));
        }
        for (auto it = node->children_.rbegin(); it < node->children_.rend(); ++it)
        {
            stack.push(*it);
        }
    }

    return root;
}
