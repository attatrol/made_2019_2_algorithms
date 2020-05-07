#include "lexer.h"
#include "grammar.h"
#include "parser.h"
#include "interpreter.h"

#include <iostream>

using namespace std;

int main()
{
    Interpreter ir(GRAMMAR);
    ir.execute("a = 1");
    ir.execute("b = 2");
    ir.execute("c = a + b");
    ir.execute("   (1+c)*8");
    ir.execute("d");
    ir.execute("(122-124)-(1*3+(12+5)*(4/3))");

    return 0;
}
