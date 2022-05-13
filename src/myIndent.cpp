#include "myIndent.h"
#include <iostream>


void Indent(unsigned int level)
{
    for (unsigned int i = 0; i < level; i++)
    {
        std::cout << "   " << "*";
    }
    std::cout << "\n";
}