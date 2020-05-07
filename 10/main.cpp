#include "lexer.h"
#include "grammar.h"
#include "parser.h"

#include <iostream>

using namespace std;

enum ValueType
{
    BOOL,
    NUM,
};

struct Value
{
    bool defined_;
    ValueType type_;
    float numValue_;
    bool boolValue_;

    Value(): defined_(false)
    {
    }
};

void processID(const ASTNode* node, const std::string& input, Value& value)
{
    assert(false);
}

void processNumber(const ASTNode* node, const std::string& input, Value& value)
{
    assert(!value.defined_);
    float& numValue = value.numValue_ = 0;
    for (std::size_t i = node->lexeme_.start_, lim = node->lexeme_.end_; i <= lim; ++i)
    {
        assert(isdigit(input[i]));
        numValue = 10 * numValue + (input[i] - '0');
    }
    value.defined_ = true;
}

void visitor(const ASTNode* node, const std::string& input, Value& value)
{
    switch (node->lexeme_.type_)
    {
    case Token::NONE:
        return;
    case Token::E:
    case Token::T:
        {
            assert(node->children_.size() == 2);
            const ASTNode* exprLeft = node->children_[0];
            const ASTNode* exprRight = node->children_[1];
            visitor(exprLeft, input, value);
            if (exprRight->lexeme_.type_ != Token::NONE && exprRight->children_[0]->lexeme_.type_ != Token::NONE)
            {
                Value rightValue;
                assert(exprRight->children_[1]->lexeme_.type_ == Token::E || exprRight->children_[1]->lexeme_.type_ == Token::T);
                visitor(exprRight->children_[1], input, rightValue);
                assert(rightValue.defined_ && value.defined_);
                switch (exprRight->children_[0]->lexeme_.type_)
                {
                case Token::PLUS:
                    value.numValue_ += rightValue.numValue_;
                    break;
                case Token::MINUS:
                    value.numValue_ -= rightValue.numValue_;
                    break;
                case Token::MUL_SIGN:
                    value.numValue_ *= rightValue.numValue_;
                    break;
                case Token::SLASH:
                    value.numValue_ /= rightValue.numValue_;
                    break;
                default:
                    assert(false);
                }
            }
        }
        return;
    case Token::E1:
    case Token::T1:
        assert(false);
        return;
    case Token::F:
        {
            const Token FFirstChildType = node->children_[0]->lexeme_.type_;
            switch (FFirstChildType)
            {
            case Token::LEFT_BRACKET:
                assert(node->children_[1]->lexeme_.type_ == Token::E);
                visitor(node->children_[1], input, value);
                return;
            case Token::ID:
                processID(node->children_[0], input, value);
                return;
            case Token::NUMBER:
                processNumber(node->children_[0], input, value);
                return;
            default:
                assert(false); // Exception
            }
        }
    default:
        assert(false);
    }
}



void test(const std::string& input)
{
    auto lexemes = getLexemes(input);
    auto firsts = getFirsts(GRAMMAR);
    auto follow = getFollow(GRAMMAR, firsts);
    auto table = buildTable(GRAMMAR, firsts, follow);

    auto ast = parse(GRAMMAR, table, lexemes);

    Value value;
    visitor(ast, input, value);

    std::cout << value.defined_ << ' ' << value.numValue_ << std::endl;
}

int main()
{
    test("(122-124)-(1*3+(12+5)*(4/3))");
    test("1 + 2");
    test("1+2*8");
    test("(1+2)*8");
    test("8*(1+2)");

    return 0;
}
