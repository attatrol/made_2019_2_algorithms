#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "grammar.h"
#include "lexeme.h"
#include "lexer.h"
#include "parser.h"

struct InterpreterValue
{
    bool defined_;
    float numValue_;

    InterpreterValue(): defined_(false)
    {
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

class Interpreter
{
private:
    const std::vector<GrammarRule> grammar_;
    const std::unordered_map<Token, std::unordered_map<Token, ParserTableEntry>> table_;
    std::unordered_map<std::string, InterpreterValue> variables_;
    std::string getVariableAssesmentName(const ASTNode* node, const std::string& input);
    void processVariable(const ASTNode* node, const std::string& input, InterpreterValue& value);
    void processNumber(const ASTNode* node, const std::string& input, InterpreterValue& value);
    void visitor(const ASTNode* node, const std::string& input, InterpreterValue& value);
public:
    Interpreter(const std::vector<GrammarRule>& grammar);
    InterpreterValue execute(const std::string& input);

//    auto ast = parse(GRAMMAR, table, lexemes);
};

#endif // INTERPRETER_H
