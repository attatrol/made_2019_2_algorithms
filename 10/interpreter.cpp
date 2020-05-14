#include <iostream>
#include "interpreter.h"

Interpreter::Interpreter(const std::vector<GrammarRule> &grammar) : grammar_(grammar), table_(buildTable(grammar))
{
}

Interpreter::~Interpreter()
{
    for (auto entry : functions_)
    {
        delete entry.second.fnTree_;
    }
}

std::string Interpreter::getVariableAssesmentName(const ASTNode *node, const std::string &input)
{
    if (!node->children_.size())
    {
        throw InterpreterException("Failed to parse left part of assignment");
    }
    ASTNode* idNode = node->children_[0];
    bool let = idNode->lexeme_.type_ == Token::LET;
    if (let)
    {
        idNode = node->children_.at(1);
    }
    else
    {
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
    }
    const std::size_t start = idNode->lexeme_.start_;
    const std::size_t size = idNode->lexeme_.end_ - start + 1;
    assert(start <= idNode->lexeme_.end_);
    std::string result = input.substr(start, size);
    bool variableNotExist = variables_.count(result) == 0;
    if (variableNotExist != let)
    {
        if (let)
        {
            throw InterpreterException("Failure to create already existing variable");
        }
        else
        {
            throw InterpreterException("Failure to assign to non-existing variable");
        }
    }
    return result;
}

void Interpreter::processVariable(const ASTNode *node, const std::string &input, InterpreterValue &value, std::unordered_map<std::string, InterpreterValue>& variables)
{
    const std::size_t start = node->lexeme_.start_;
    const std::size_t size = node->lexeme_.end_ - start + 1;
    assert(start <= node->lexeme_.end_);
    std::string variableName = input.substr(start, size);
    if (!variables.count(variableName))
    {
        throw InterpreterException("Unknown variable \"" + variableName + "\"");
    }
    value = variables[variableName];
}

void Interpreter::processNumber(const ASTNode *node, const std::string &input, InterpreterValue &value)
{
    const ASTNode *numNode0 = node->children_.at(0);
    float& numValue = value.re_ = 0;
    assert(numNode0->lexeme_.start_ <= numNode0->lexeme_.end_);
    for (std::size_t i = numNode0->lexeme_.start_, lim = numNode0->lexeme_.end_; i <= lim; ++i)
    {
        assert(isdigit(input[i]));
        numValue = 10 * numValue + (input[i] - '0');
    }
    if (node->children_.at(1)->children_.at(0)->lexeme_.type_ == Token::POINT)
    {
        const ASTNode *numNode1 = node->children_.at(1)->children_.at(1);
        assert(numNode1->lexeme_.type_ == Token::NUMBER);
        float numerator = 0;
        float denominator = 1;
        assert(numNode1->lexeme_.start_ <= numNode1->lexeme_.end_);
        for (std::size_t i = numNode1->lexeme_.start_, lim = numNode1->lexeme_.end_; i <= lim; ++i)
        {
            assert(isdigit(input[i]));
            numerator = 10 * numerator + (input[i] - '0');
            denominator *= 10;
        }
        numValue += numerator / denominator;
    }
    value.defined_ = true;
    value.im_ = 0;
}

void Interpreter::processComplexNumber(const ASTNode *node, const std::string &input, InterpreterValue &value, std::unordered_map<std::string, InterpreterValue>& variables, std::size_t stackDepth)
{
    InterpreterValue reValue;
    visitor(node->children_.at(1), input, reValue, variables, stackDepth);
    if (reValue.isComplexNumber())
    {
        throw InterpreterException("Real part of a complex number must be presented as a non-complex number");
    }
    InterpreterValue imValue;
    visitor(node->children_.at(3), input, imValue, variables, stackDepth);
    if (imValue.isComplexNumber())
    {
        throw InterpreterException("Imaginary part of the number must be presented as a non-complex number");
    }
    value.re_ = reValue.re_;
    value.im_ = imValue.re_;
    value.defined_ = true;
}

void Interpreter::getRealPart(const ASTNode *node, const std::string &input, InterpreterValue &value, std::unordered_map<std::string, InterpreterValue> &variables, std::size_t stackDepth)
{
    visitor(node->children_.at(2), input, value, variables, stackDepth);
    if (!value.defined_)
    {
        throw InterpreterException("Falure to get real part of undefined");
    }
    value.im_ = 0;
}

void Interpreter::getImaginaryPart(const ASTNode *node, const std::string &input, InterpreterValue &value, std::unordered_map<std::string, InterpreterValue> &variables, std::size_t stackDepth)
{
    visitor(node->children_.at(2), input, value, variables, stackDepth);
    if (!value.defined_)
    {
        throw InterpreterException("Falure to get imaginary part of undefined");
    }
    value.re_ = value.im_;
    value.im_ = 0;
}


void Interpreter::registerFunction(const ASTNode *node, const std::string &input)
{
    const ASTNode* fnNode = node->children_.at(1);
    // fnName
    const ASTNode* fnNameNode = fnNode->children_.at(0);
    const std::size_t start = fnNameNode->lexeme_.start_;
    const std::size_t size = fnNameNode->lexeme_.end_ - start + 1;
    assert(start <= fnNameNode->lexeme_.end_);
    const std::string fnName = input.substr(start, size);
    if (functions_.count(fnName))
    {
        throw("Function with name \"" + fnName + "\" already registered");
    }
    // argNames
    const ASTNode* argsNode = fnNode->children_.at(2);
    std::vector<std::string> argNames;
    while(argsNode->lexeme_.type_ != Token::NONE)
    {
        const ASTNode* argNameNode = argsNode->children_.at(0);
        if (argNameNode->lexeme_.type_ == Token::COMMA)
        {
            argNameNode = argsNode->children_.at(1);
        }
        else if (argNameNode->lexeme_.type_ == Token::NONE)
        {
            break;
        }
        assert(argNameNode->lexeme_.type_ == Token::VARIABLE);
        const std::size_t start = argNameNode->lexeme_.start_;
        const std::size_t size = argNameNode->lexeme_.end_ - start + 1;
        assert(start <= argNameNode->lexeme_.end_);
        argNames.push_back(input.substr(start, size));
        argsNode = argsNode->children_.back();
    }
    // copy AST
    const ASTNode* bodyNode = node->children_.at(2);
    assert(bodyNode->lexeme_.type_ == Token::R_ASSIGN);
    if (bodyNode->children_.at(0)->lexeme_.type_ == Token::NONE)
    {
        throw InterpreterException("Failure to define a function without the body");
    }
    bodyNode = bodyNode->children_.at(1);
    assert(bodyNode->lexeme_.type_ == Token::EXPR);
    ASTNode* bodyCopy = new ASTNode(*bodyNode);

    functions_.insert({ fnName, {
                            bodyCopy,
                            input,
                            argNames
                        } });
}

FunctionContext Interpreter::getFunctionContext(const ASTNode *node, const std::string &input)
{
    const std::size_t start = node->lexeme_.start_;
    const std::size_t size = node->lexeme_.end_ - start + 1;
    assert(start <= node->lexeme_.end_);
    std::string fnName = input.substr(start, size);
    if (!functions_.count(fnName))
    {
        throw InterpreterException("Unknown function name \"" + fnName + "\"");
    }
    return functions_[fnName];
}

void getExpressionNodes(const ASTNode *node, std::vector<const ASTNode*>& expressions)
{
    for (const ASTNode* child : node->children_)
    {
        if (child->lexeme_.type_ == Token::EXPR)
        {
            expressions.push_back(child);
        }
        else
        {
            getExpressionNodes(child, expressions);
        }
    }
}

std::vector<InterpreterValue> Interpreter::processCallValues(const ASTNode *node, const std::string &input, std::unordered_map<std::string, InterpreterValue>& variables, std::size_t stackDepth)
{
    // get all Expression nodes
    std::vector<const ASTNode*> expressions;
    getExpressionNodes(node, expressions);
    // map expressions into values
    std::vector<InterpreterValue> result(expressions.size());
    for (std::size_t i = 0, size = expressions.size(); i < size; ++i)
    {
        visitor(expressions[i], input, result[i], variables, stackDepth);
        if (!result[i].defined_)
        {
            throw InterpreterException("Function argument value undefined");
        }
    }
    return result;
}

std::unordered_map<std::string, InterpreterValue> Interpreter::buildCallContext(const FunctionContext &fnContext, const std::vector<InterpreterValue> &callValues)
{
    if(fnContext.argNames_.size() != callValues.size())
    {
        throw InterpreterException("Argument count mismatch in function call");
    }
    std::unordered_map<std::string, InterpreterValue> result;
    for (std::size_t i = 0, size = callValues.size(); i < size; ++i)
    {
        result.insert({fnContext.argNames_[i], callValues[i]});
    }
    return result;
}


void Interpreter::visitor(const ASTNode *node, const std::string &input,
                          InterpreterValue &value, std::unordered_map<std::string,
                          InterpreterValue>& variables, std::size_t stackDepth)
{
    if (stackDepth > MAX_STACK_DEPTH)
    {
        throw InterpreterException("Max stack depth exceeded");
    }
    switch (node->lexeme_.type_)
    {
    case Token::NONE:
        return;
    case Token::STMT:
    {
        if (node->children_.back()->children_.at(0)->lexeme_.type_ == Token::EQ) // assignment
        {
            if (node->children_.at(0)->lexeme_.type_ == Token::DEF)
            {
                registerFunction(node, input); // value is undefined
            }
            else
            {
                std::string varName = getVariableAssesmentName(node, input);
                visitor(node->children_.back()->children_.at(1), input, value, variables, stackDepth);
                variables[varName] = value;
            }
        }
        else if (node->children_.at(0)->lexeme_.type_ == Token::LET) // undefined variable declaration
        {
            std::string varName = getVariableAssesmentName(node, input);
            variables[varName] = value;

        }
        else // general expression
        {
            visitor(node->children_.at(0), input, value, variables, stackDepth);
        }
    }
        return;
    case Token::EXPR:
    case Token::SUM_L:
    {
        assert(node->children_.size() == 2);
        const ASTNode* exprLeft = node->children_.at(0);
        const ASTNode* exprRight = node->children_.at(1);
        visitor(exprLeft, input, value, variables, stackDepth);
        if (exprRight->lexeme_.type_ != Token::NONE && exprRight->children_.at(0)->lexeme_.type_ != Token::NONE)
        {
            InterpreterValue rightValue;
            assert(exprRight->children_.at(1)->lexeme_.type_ == Token::EXPR || exprRight->children_.at(1)->lexeme_.type_ == Token::SUM_L);
            visitor(exprRight->children_.at(1), input, rightValue, variables, stackDepth);
            if(!rightValue.defined_ || !value.defined_)
            {
                throw InterpreterException("Failed to execute arithmetic operation over undefined values");
            }
            switch (exprRight->children_.at(0)->lexeme_.type_)
            {
            case Token::PLUS:
                value += rightValue;
                break;
            case Token::MINUS:
                value -= rightValue;
                break;
            case Token::MUL_SIGN:
                value *= rightValue;
                break;
            case Token::SLASH:
                value /= rightValue;
                break;
            default:
                throw InterpreterException("Unknown operation token");
            }
        }
    }
        return;
    case Token::R_ASSIGN:
    case Token::SUM_R:
    case Token::PROD_R:
        throw InterpreterException("Unexpected token");
        return;
    case Token::PROD_L:
    {
        const Token FFirstChildType = node->children_.at(0)->lexeme_.type_;
        switch (FFirstChildType)
        {
        case Token::TERM:
            visitor(node->children_.at(0), input, value, variables, stackDepth);
            return;
        case Token::PLUS:
            visitor(node->children_.at(1), input, value, variables, stackDepth);
            if(!value.defined_)
            {
                throw InterpreterException("Failed to execute arithmetic operation over undefined values");
            }
            return;
        case Token::MINUS:
            visitor(node->children_.at(1), input, value, variables, stackDepth);
            if(!value.defined_)
            {
                throw InterpreterException("Failed to execute arithmetic operation over undefined values");
            }
            value.re_ = -value.re_;
            value.im_ = -value.im_;
            return;
        default:
            throw InterpreterException("Unexpected token");
        }
    }
    case Token::TERM:
    {
        const Token FFirstChildType = node->children_.at(0)->lexeme_.type_;
        switch (FFirstChildType)
        {
        case Token::LEFT_BRACKET:
            assert(node->children_.at(1)->lexeme_.type_ == Token::EXPR);
            visitor(node->children_.at(1), input, value, variables, stackDepth);
            return;
        case Token::VARIABLE:
            assert(node->children_.size() == 2);
            assert(node->children_.at(1)->lexeme_.type_ == Token::CALL);
            if (node->children_.at(1)->children_.at(0)->lexeme_.type_ == Token::NONE)
            {
                processVariable(node->children_.at(0), input, value, variables);
            }
            else
            {
                FunctionContext fnContext = getFunctionContext(node->children_.at(0), input);
                std::vector<InterpreterValue> callValues = processCallValues(node->children_.at(1), input, variables, stackDepth);
                std::unordered_map<std::string, InterpreterValue> callContext = buildCallContext(fnContext, callValues);
                visitor(fnContext.fnTree_, fnContext.input_, value, callContext, stackDepth + 1);
            }
            return;
        case Token::NUMBER:
            processNumber(node, input, value);
            return;
        case Token::LEFT_SQR_BRACKET:
            processComplexNumber(node, input, value, variables, stackDepth);
            return;
        case Token::RE:
            getRealPart(node, input, value, variables, stackDepth);
            return;
        case Token::IM:
            getImaginaryPart(node, input, value, variables, stackDepth);
            return;
        default:
            throw InterpreterException("Unexpected token");
        }
    }
    default:
        throw InterpreterException("Unexpected token");
    }
}

InterpreterResult Interpreter::execute(const std::string &input, bool echo, bool silent)
{
    ASTNode* ast = nullptr;
    try
    {
        std::vector<Lexeme> lexemes = getLexemes(input);
        ast = parse(grammar_, table_, lexemes);
        InterpreterValue value;
        visitor(ast, input, value, variables_, 0);
        delete ast;
        if (echo && !silent)
        {
            if (value.defined_)
            {
                if (value.isComplexNumber())
                {
                    std::cout << '[' << value.re_ << "; " << value.im_ << "]\n";
                }
                else
                {
                    std::cout << value.re_ << '\n';
                }
            }
            else
            {
                std::cout << "Undefined\n";
            }
        }
        return { InterpreterErrorCode::NO_ERROR, value };
    }
    catch (const LexerException& e)
    {
        if (!silent)
        {
            std::cout << "Lexer encountered unknown symbol at " << e.pos_ << '\n';
        }
        return { InterpreterErrorCode::LEXER, { } };
    }
    catch (const ParserException& e)
    {
        delete ast;
        if (!silent)
        {
            std::cout << "Parser encountered unknown symbol at lexeme #" << e.pos_ << '\n';
        }
        return { InterpreterErrorCode::PARSER, { } };
    }
    catch (const InterpreterException& e)
    {
        delete ast;
        if (!silent)
        {
            std::cout << "Interpreter error: " << e.message_ << '\n';
        }
        return { InterpreterErrorCode::INTERPRETER, { } };
    }
}
