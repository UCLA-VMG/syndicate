#include <iostream>
#include <Spinnaker.h>

int main(int, char**) {
    std::cout << "Hello, world!\n";
    Spinnaker::SystemPtr system = Spinnaker::System::GetInstance();
    std::cout << "Hello, world!\n";
}