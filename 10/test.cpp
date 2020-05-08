#include <iostream>
#include "interpreter.h"

struct TestInput
{
    InterpreterResult result_;
    const std::string input_;
};

static const std::string PARSER_ERRORS[] =
{
    ""
};

static const TestInput VALID_ONE_LINERS[] =
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
    { { InterpreterErrorCode::NO_ERROR, { true, 0 } }, "-1 + 1" },
    { { InterpreterErrorCode::NO_ERROR, { true, 0 } }, "-1 + 1" },
    { { InterpreterErrorCode::NO_ERROR, { true, 0 } }, "-1 + 1" },
    { { InterpreterErrorCode::NO_ERROR, { true, 0 } }, "-1 + 1" },
    { { InterpreterErrorCode::NO_ERROR, { true, 0 } }, "-1 + 1" },
    { { InterpreterErrorCode::NO_ERROR, { true, 0 } }, "-1 + 1" },
};

bool runOneLiners()
{
    std::size_t i = 0;
    for (const TestInput& input : VALID_ONE_LINERS)
    {
        Interpreter ir;
        InterpreterResult res = ir.execute(input.input_); // TODO : false
        if (res != input.result_)
        {
            std::cout << "Invalid one liner " << i << '\n';
            return false;
        }
        ++i;
    }
    return true;
}


int main()
{
    Interpreter ir;
    ir.execute("def f(x) = x + 1");
    ir.execute("def g(x, y) = x + y");
    ir.execute("def h() = 42");
    ir.execute("f(10,)");
    ir.execute("let a = -1");
    ir.execute("f(a,)");
    ir.execute("g(a, a,)");
    ir.execute("h()");

//    runOneLiners();

    return 0;
}
