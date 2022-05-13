#include <iostream>
#include <Spinnaker.h>
#include "myIndent.h"


int main(int, char**) {
    std::cout << "Hello, world!\n";
    Spinnaker::SystemPtr system = Spinnaker::System::GetInstance();
    Indent(5);
    std::cout << "Hello, world!\n";
}