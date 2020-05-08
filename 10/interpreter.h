#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "grammar.h"
#include "lexeme.h"
#include "lexer.h"
#include "parser.h"

enum InterpreterErrorCode
{
    NO_ERROR,
    PARSER,
    LEXER,
    INTERPRETER
};

struct InterpreterValue
{
    bool defined_;
    float numValue_;

    InterpreterValue(): defined_(false)
    {
    }
    InterpreterValue(bool defined, float numValue): defined_(defined), numValue_(numValue)
    {
    }
    inline bool operator==(const InterpreterValue& rhs) const
    {
        return defined_ ? (defined_ == rhs.defined_ && numValue_ == rhs.numValue_) : !rhs.defined_;
    }
    inline bool operator!=(const InterpreterValue& rhs) const
    {
        return defined_ ? (defined_ != rhs.defined_ || numValue_ != rhs.numValue_) : rhs.defined_;
    }
};

struct InterpreterResult
{
    InterpreterErrorCode errorCode_;
    InterpreterValue value_;

    inline bool operator==(const InterpreterResult& rhs) const
    {
        return errorCode_ == rhs.errorCode_ && value_ == rhs.value_;
    }
    inline bool operator!=(const InterpreterResult& rhs) const
    {
        return errorCode_ != rhs.errorCode_ || value_ != rhs.value_;
    }
};


struct InterpreterException : public std::exception
{
    const std::string message_;

    InterpreterException(const std::string& message): message_(message)
    {
    }

    const char* what() const noexcept override
    {
        return "Error during a statement interpretation";
    }
};

struct FunctionContext
{
    ASTNode* fnTree_;
    std::string input_;
    std::vector<std::string> argNames_;
};

class Interpreter
{
private:
    const std::vector<GrammarRule> grammar_;
    const std::unordered_map<Token, std::unordered_map<Token, ParserTableEntry>> table_;
    std::unordered_map<std::string, InterpreterValue> variables_;
    std::unordered_map<std::string, FunctionContext> functions_;
    std::string getVariableAssesmentName(const ASTNode* node, const std::string& input);
    void processVariable(const ASTNode* node, const std::string& input, InterpreterValue& value, std::unordered_map<std::string, InterpreterValue> &variables);
    void processNumber(const ASTNode* node, const std::string& input, InterpreterValue& value);
    void registerFunction(const ASTNode* node, const std::string& input);
    FunctionContext getFunctionContext(const ASTNode* node, const std::string &input);
    std::vector<InterpreterValue>processCallValues(const ASTNode* node, const std::string &input, std::unordered_map<std::string, InterpreterValue>& variables);
    std::unordered_map<std::string, InterpreterValue> buildCallContext(const FunctionContext& fnContext, const std::vector<InterpreterValue>& callValues);
    void visitor(const ASTNode* node, const std::string& input, InterpreterValue& value, std::unordered_map<std::string, InterpreterValue>& variables);
public:
    Interpreter(const std::vector<GrammarRule>& grammar = GRAMMAR);
    InterpreterResult execute(const std::string& input, bool cout = true);

//    auto ast = parse(GRAMMAR, table, lexemes);
};

#endif // INTERPRETER_H
