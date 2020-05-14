#include <iostream>
#include "interpreter.h"

struct TestInput
{
    InterpreterResult result_;
    const std::string input_;
};

static const std::string LEXER_ERRORS[] =
{
    "#", // unknown symbol
    "abracadabra#", // unknown symbol
    "abracadabra # 111", // unknown symbol
};

bool runLexerErrors()
{
    std::size_t i = 0;
    for (const std::string& input : LEXER_ERRORS)
    {
        Interpreter ir;
        InterpreterResult res = ir.execute(input, false, true);
        if (res.errorCode_ != InterpreterErrorCode::LEXER)
        {
            std::cout << "Invalid lexer error " << i << '\n';
            return false;
        }
        ++i;
    }
    return true;
}

static const std::string PARSER_ERRORS[] =
{
    "",
    "(",
    "()",
    "[14; 5)",
    "1 + 1 (",
    "()1 + 1",
    "1 */ 1",
    "[[1;1]",
    "a(4,",
    "a 4",
    "4 a",
    "(a) (b)",
    "a(+)b",
    "let a + b = 1", // left side of assignment must be a variable name
};

bool runParserErrors()
{
    std::size_t i = 0;
    for (const std::string& input : PARSER_ERRORS)
    {
        Interpreter ir;
        InterpreterResult res = ir.execute(input, false, true);
        if (res.errorCode_ != InterpreterErrorCode::PARSER)
        {
            std::cout << "Invalid parser error " << i << '\n';
            return false;
        }
        ++i;
    }
    return true;
}

static const std::vector<TestInput> VALID_ONE_LINERS =
{
    { { InterpreterErrorCode::NO_ERROR, { true, 0 } }, "-1 + 1" },
    { { InterpreterErrorCode::NO_ERROR, { true, 0 } }, "0" },
    { { InterpreterErrorCode::NO_ERROR, { true, 0 } }, "-0" },
    { { InterpreterErrorCode::NO_ERROR, { true, 0 } }, "((-1) + -(-(1)))" },
    { { InterpreterErrorCode::NO_ERROR, { true, 0 } }, "0.0" },
    { { InterpreterErrorCode::NO_ERROR, { true, -0.25 } }, "-0.25" },
    { { InterpreterErrorCode::NO_ERROR, { true, 1 } }, "let x = 1" },
    { { InterpreterErrorCode::NO_ERROR, { false, 0 } }, "def f(x) = x + 1" },
    { { InterpreterErrorCode::NO_ERROR, { false, 0 } }, "def f(x, y) = x * y" },
    { { InterpreterErrorCode::NO_ERROR, { false, 0 } }, "def zero() = 0" },
    { { InterpreterErrorCode::NO_ERROR, { true, 4 } }, "let a1 = (1 + 1) * (1 + 1)" },
    { { InterpreterErrorCode::NO_ERROR, { true, -2 } }, "-((1 + 1) * (2 - 1))" },
    { { InterpreterErrorCode::NO_ERROR, { true, std::numeric_limits<float>::infinity() } }, "1 / (10 - (1 + 1) * 5)" },
    { { InterpreterErrorCode::NO_ERROR, { true, 1, -1 } }, "let b = [1; -1]" },
    { { InterpreterErrorCode::NO_ERROR, { true, 1, 1 } }, "-[Im([1; -1]); -Re([1; -1])]" },
    { { InterpreterErrorCode::NO_ERROR, { false, 0 } }, "let a" },
    { { InterpreterErrorCode::NO_ERROR, { true, 0, 8 } }, "[2; 2] * [2; 2]" },
    { { InterpreterErrorCode::NO_ERROR, { true, -5, 1 } }, "-[15; 3] / --3" },
    { { InterpreterErrorCode::NO_ERROR, { true, 1 } }, "-Im([15; 3] / --3)" },
    { { InterpreterErrorCode::NO_ERROR, { true, 0 } }, "Im(42)" },
    { { InterpreterErrorCode::NO_ERROR, { true, 6 } }, "2 + 2 * 2" },
    { { InterpreterErrorCode::NO_ERROR, { true, 8 } }, "(2 + 2) * 2" },
    { { InterpreterErrorCode::NO_ERROR, { true, 2 } }, "-(2) + ((2) * (2))" },
    { { InterpreterErrorCode::NO_ERROR, { true, 0.24f, -0.68f } }, "[3; 2] / [4; -3]" },
};

bool runOneLiners()
{
    std::size_t i = 0;
    for (const TestInput& input : VALID_ONE_LINERS)
    {
        Interpreter ir;
        InterpreterResult res = ir.execute(input.input_, false);
        if (res != input.result_)
        {
            std::cout << "Invalid one liner " << i << '\n';
            return false;
        }
        ++i;
    }
    return true;
}

static const std::vector<TestInput> VALID_SEQUENCE_1 =
{
    { { InterpreterErrorCode::NO_ERROR, { false, 0 } }, "def f(x) = x + 1" },
    { { InterpreterErrorCode::NO_ERROR, { false, 0 } }, "def g(x, y) = x + y" },
    { { InterpreterErrorCode::NO_ERROR, { false, 0 } }, "def h() = 42" },
    { { InterpreterErrorCode::NO_ERROR, { true, 11 } }, "f(10)" },
    { { InterpreterErrorCode::NO_ERROR, { true, -1 } }, "let a = -1" },
    { { InterpreterErrorCode::NO_ERROR, { true, 0 } }, "f(a)" },
    { { InterpreterErrorCode::NO_ERROR, { true, -2 } }, "g(a, a)" },
    { { InterpreterErrorCode::NO_ERROR, { true, 42 } }, "h()" },
    { { InterpreterErrorCode::NO_ERROR, { true, 46 } }, "a = g(h(), a + 5)" },
};

static const std::vector<TestInput> VALID_SEQUENCE_2 =
{
    { { InterpreterErrorCode::NO_ERROR, { false, 0 } }, "def g(a) = (0.25 * a + 1) * a - [0.5; 0.5]" },
    { { InterpreterErrorCode::NO_ERROR, { false, 0 } }, "def f(a, b) = g(a) / g(b)" },
    { { InterpreterErrorCode::NO_ERROR, { true, 1, 1 } }, "let a = [1; 1]" },
    { { InterpreterErrorCode::NO_ERROR, { true, -2, -2 } }, "let b = [-2; -2]" },
    { { InterpreterErrorCode::NO_ERROR, { true, -1.4f, -1.8f } }, "f(b, a)" },
};

static const std::vector<TestInput> INTERPRETER_ERRORS =
{
    { { InterpreterErrorCode::INTERPRETER, { false, 0 } }, "a + b = 1" }, // left side of assignment must be a variable name
    { { InterpreterErrorCode::NO_ERROR, { false, 0 } }, "def f() = f() + 42" },
    { { InterpreterErrorCode::INTERPRETER, { false, 0 } }, "f()" }, // recursion is forbidden
    { { InterpreterErrorCode::NO_ERROR, { false, 0 } }, "let a" },
    { { InterpreterErrorCode::INTERPRETER, { false, 0 } }, "let a  = 1" }, // second let is forbidden
    { { InterpreterErrorCode::INTERPRETER, { false, 0 } }, "b = 1" }, // assignment to unknown variable is forbidden
    { { InterpreterErrorCode::INTERPRETER, { false, 0 } }, "c" }, // unknown variable
    { { InterpreterErrorCode::NO_ERROR, { false, 0 } }, "def g(x, y) = x + y" },
    { { InterpreterErrorCode::INTERPRETER, { false, 0 } }, "f(0)" }, // arguments mismatch
    { { InterpreterErrorCode::INTERPRETER, { false, 0 } }, "g(1)" }, // arguments mismatch
    { { InterpreterErrorCode::INTERPRETER, { false, 0 } }, "g(0, 1, 2)" }, // arguments mismatch
    { { InterpreterErrorCode::NO_ERROR, { false, 0 } }, "let b" },
    { { InterpreterErrorCode::INTERPRETER, { false, 0 } }, "g(b, 2)" }, // usage of undefined variable "b"
    { { InterpreterErrorCode::INTERPRETER, { false, 0 } }, "[1; [2; 3]]" }, // imaginary part must be real
    { { InterpreterErrorCode::INTERPRETER, { false, 0 } }, "[[1; 2]; 3]" }, // real part must be real
    { { InterpreterErrorCode::NO_ERROR, { false, 0 } }, "let c" },
    { { InterpreterErrorCode::INTERPRETER, { false, 0 } }, "b = b + c" }, // usage of undefined variables in the arithmetics
    { { InterpreterErrorCode::INTERPRETER, { false, 0 } }, "b = -c" }, // usage of undefined variables in the arithmetics
};

bool runSequence(const std::vector<TestInput>& sequence)
{
    std::size_t i = 0;
    Interpreter ir;
    for (const TestInput& input : sequence)
    {
        InterpreterResult res = ir.execute(input.input_, false, true); // TODO : false
        if (res != input.result_)
        {
            std::cout << "Invalid sequence result at " << i << '\n';
            return false;
        }
        ++i;
    }
    return true;
}

int main()
{
    if (runLexerErrors()
     && runParserErrors()
     && runOneLiners()
     && runSequence(VALID_SEQUENCE_1)
     && runSequence(VALID_SEQUENCE_2)
     && runSequence(INTERPRETER_ERRORS)
     )
    {
        std::cout << "Test run successful!\n";
    }

    return 0;
}
