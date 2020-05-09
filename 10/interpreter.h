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

struct InterpreterValue
{
    bool defined_;
    float re_;
    float im_;

    bool isComplexNumber()
    {
        return im_ != 0;
    }

    InterpreterValue(): defined_(false)
    {
    }
    InterpreterValue(bool defined, float numValue): defined_(defined), re_(numValue), im_(0)
    {
    }
    InterpreterValue(bool defined, float re, float im): defined_(defined), re_(re), im_(im)
    {
    }
    InterpreterValue(const InterpreterValue& other): defined_(other.defined_), re_(other.re_), im_(other.im_)
    {
    }
    inline bool operator==(const InterpreterValue& rhs) const
    {
        return defined_ ? (defined_ == rhs.defined_ && re_ == rhs.re_ && im_ == rhs.re_) : !rhs.defined_;
    }
    inline bool operator!=(const InterpreterValue& rhs) const
    {
        return defined_ ? (defined_ != rhs.defined_ || re_ != rhs.re_ || im_ != rhs.im_) : rhs.defined_;
    }
    InterpreterValue& operator+=(const InterpreterValue& rhs)
    {
        if (!defined_ || !rhs.defined_)
        {
            throw new InterpreterException("Failure to perform addition on undefined values");
        }
        re_ += rhs.re_;
        im_ += rhs.im_;
        return *this;
    }
    InterpreterValue& operator-=(const InterpreterValue& rhs)
    {
        if (!defined_ || !rhs.defined_)
        {
            throw new InterpreterException("Failure to perform substraction on undefined values");
        }
        re_ -= rhs.re_;
        im_ -= rhs.im_;
        return *this;
    }
    InterpreterValue& operator*=(const InterpreterValue& rhs)
    {
        if (!defined_ || !rhs.defined_)
        {
            throw new InterpreterException("Failure to perform multiplication on undefined values");
        }
        float re = rhs.re_ * re_ - rhs.im_ * im_;
        im_ = rhs.im_ * re_ + rhs.re_ * im_;
        re_ = re;
        return *this;
    }
    InterpreterValue& operator/=(const InterpreterValue& rhs)
    {
        if (!defined_ || !rhs.defined_)
        {
            throw new InterpreterException("Failure to perform division on undefined values");
        }
        if (im_ || rhs.im_)
        {
            float re = rhs.re_ * re_ + rhs.im_ * im_;
            float im = rhs.im_ * re_ - rhs.re_ * im_;
            float denominator = rhs.re_ * rhs.re_ + rhs.im_ * rhs.im_;
            re_ = re / denominator;
            im_ = im / denominator;
        }
        else
        {
            re_ /= rhs.re_;
        }
        return *this;
    }
    InterpreterValue operator+(const InterpreterValue& rhs) const &
    {
        InterpreterValue result(*this);
        result += rhs;
        return result;
    }
    InterpreterValue operator+(const InterpreterValue& rhs) &&
    {
        *this += rhs;
        return std::move(*this);
    }
    InterpreterValue operator-(const InterpreterValue& rhs) const &
    {
        InterpreterValue result(*this);
        result -= rhs;
        return result;
    }
    InterpreterValue operator-(const InterpreterValue& rhs) &&
    {
        *this -= rhs;
        return std::move(*this);
    }
    InterpreterValue operator*(const InterpreterValue& rhs) const &
    {
        InterpreterValue result(*this);
        result *= rhs;
        return result;
    }
    InterpreterValue operator*(const InterpreterValue& rhs) &&
    {
        *this *= rhs;
        return std::move(*this);
    }
    InterpreterValue operator/(const InterpreterValue& rhs) const &
    {
        InterpreterValue result(*this);
        result /= rhs;
        return result;
    }
    InterpreterValue operator/(const InterpreterValue& rhs) &&
    {
        *this /= rhs;
        return std::move(*this);
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

struct FunctionContext
{
    ASTNode* fnTree_;
    std::string input_;
    std::vector<std::string> argNames_;
};

static const std::size_t MAX_STACK_DEPTH = 50;

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
    void processComplexNumber(const ASTNode *node, const std::string &input, InterpreterValue &value, std::unordered_map<std::string, InterpreterValue>& variables, std::size_t stackDepth);
    void getRealPart(const ASTNode *node, const std::string &input, InterpreterValue &value, std::unordered_map<std::string, InterpreterValue>& variables, std::size_t stackDepth);
    void getImaginaryPart(const ASTNode *node, const std::string &input, InterpreterValue &value, std::unordered_map<std::string, InterpreterValue>& variables, std::size_t stackDepth);
    void registerFunction(const ASTNode* node, const std::string& input);
    FunctionContext getFunctionContext(const ASTNode* node, const std::string &input);
    std::vector<InterpreterValue>processCallValues(const ASTNode* node, const std::string &input, std::unordered_map<std::string, InterpreterValue>& variables, std::size_t stackDepth);
    std::unordered_map<std::string, InterpreterValue> buildCallContext(const FunctionContext& fnContext, const std::vector<InterpreterValue>& callValues);
    void visitor(const ASTNode* node, const std::string& input, InterpreterValue& value, std::unordered_map<std::string, InterpreterValue>& variables, std::size_t stackDepth);
public:
    Interpreter(const std::vector<GrammarRule>& grammar = GRAMMAR);
    ~Interpreter();
    Interpreter(const Interpreter&) = delete;
    Interpreter(Interpreter&&) = delete;
    Interpreter& operator=(const Interpreter&) = delete;
    Interpreter& operator=(Interpreter&&) = delete;
    InterpreterResult execute(const std::string& input, bool echo = true, bool silent = false);
};

#endif // INTERPRETER_H
