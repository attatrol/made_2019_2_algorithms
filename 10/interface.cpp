#include <iostream>
#include "interpreter.h"

int main()
{
    Interpreter ir;
    std::string input;
    while (std::getline(std::cin, input))
    {
        ir.execute(input);
    }
    return 0;
}
