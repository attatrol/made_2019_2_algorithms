#include <iostream>
#include "interpreter.h"

Interpreter::Interpreter(const std::vector<GrammarRule> &grammar) : grammar_(grammar), table_(buildTable(grammar))
{
}

std::string Interpreter::getVariableAssesmentName(const ASTNode *node, const std::string &input)
{
    if (!node->children_.size())
    {
        throw InterpreterException("Failed to parse left part of assignment");
    }
    ASTNode* idNode = node->children_[0];
    if (!idNode->children_.size())
    {
        throw InterpreterException("Failed to parse left part of assignment");
    }
    idNode = idNode->children_[0];
    if (!idNode->children_.size())
    {
        throw InterpreterException("Failed to parse left part of assignment");
    }
    idNode = idNode->children_[0];
    if (idNode->lexeme_.type_ != Token::VARIABLE)
    {
        throw InterpreterException("Failed to parse left part of assignment");
    }
    const std::size_t start = idNode->lexeme_.start_;
    const std::size_t size = idNode->lexeme_.end_ - start + 1;
    assert(start <= idNode->lexeme_.end_);
    return input.substr(start, size);
}

void Interpreter::processVariable(const ASTNode *node, const std::string &input, InterpreterValue &value)
{
    const std::size_t start = node->lexeme_.start_;
    const std::size_t size = node->lexeme_.end_ - start + 1;
    assert(start <= node->lexeme_.end_);
    std::string variableName = input.substr(start, size);
    if (!variables_.count(variableName))
    {
        throw InterpreterException("Unknown variable \"" + variableName + "\"");
    }
    value = variables_[variableName];
}

void Interpreter::processNumber(const ASTNode *node, const std::string &input, InterpreterValue &value)
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

void Interpreter::visitor(const ASTNode *node, const std::string &input, InterpreterValue &value)
{
    switch (node->lexeme_.type_)
    {
    case Token::NONE:
        return;
    case Token::S:
    {
        if (node->children_.at(1)->children_.at(0)->lexeme_.type_ == Token::EQ)
        {
            std::string varName = getVariableAssesmentName(node->children_.at(0), input);
            visitor(node->children_.at(1)->children_.at(1), input, value);
            variables_[varName] = value;
        }
        else
        {
            visitor(node->children_.at(0), input, value);
        }
    }
        return;
    case Token::E:
    case Token::T:
    {
        assert(node->children_.size() == 2);
        const ASTNode* exprLeft = node->children_.at(0);
        const ASTNode* exprRight = node->children_.at(1);
        visitor(exprLeft, input, value);
        if (exprRight->lexeme_.type_ != Token::NONE && exprRight->children_.at(0)->lexeme_.type_ != Token::NONE)
        {
            InterpreterValue rightValue;
            assert(exprRight->children_.at(1)->lexeme_.type_ == Token::E || exprRight->children_.at(1)->lexeme_.type_ == Token::T);
            visitor(exprRight->children_.at(1), input, rightValue);
            assert(rightValue.defined_ && value.defined_);
            switch (exprRight->children_.at(0)->lexeme_.type_)
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
    case Token::S1:
    case Token::E1:
    case Token::T1:
        assert(false);
        return;
    case Token::F:
    {
        const Token FFirstChildType = node->children_.at(0)->lexeme_.type_;
        switch (FFirstChildType)
        {
        case Token::LEFT_BRACKET:
            assert(node->children_.at(1)->lexeme_.type_ == Token::E);
            visitor(node->children_.at(1), input, value);
            return;
        case Token::VARIABLE:
            processVariable(node->children_.at(0), input, value);
            return;
        case Token::NUMBER:
            processNumber(node->children_.at(0), input, value);
            return;
        default:
            throw InterpreterException("Unknown operation token");
        }
    }
    default:
        throw InterpreterException("Unknown token");
    }
}

InterpreterValue Interpreter::execute(const std::string &input)
{
    ASTNode* ast = nullptr;
    try
    {
        std::vector<Lexeme> lexemes = getLexemes(input);
        ast = parse(grammar_, table_, lexemes);
        InterpreterValue value;
        visitor(ast, input, value);
        delete ast;
        if (value.defined_)
        {
            std::cout << value.numValue_ << '\n';
        }
        else
        {
            std::cout << "Undefined\n";
        }
        return value;
    }
    catch (const LexerException& e)
    {
        std::cout << "Lexer encountered unknown symbol at " << e.pos_ << '\n';
    }
    catch (const ParserException& e)
    {
        delete ast;
        std::cout << "Parser encountered unknown symbol at lexeme #" << e.pos_ << '\n';
    }
    catch (const InterpreterException& e)
    {
        delete ast;
        std::cout << "Interpreter error: " << e.message_ << '\n';
    }
    return InterpreterValue(); // return undefined value
}
