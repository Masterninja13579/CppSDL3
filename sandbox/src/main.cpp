
//#include "experiments/bgfxTriangle.h"
#include "experiments/stateMachineTests.h"

#include <iostream>

int main(int argc, char** argv)
{
    try
    {
       int result = runTest();
       std::cout << (result == EXIT_SUCCESS ? "Success\n" : "Failed\n");
       return result;
    }
    catch(const std::exception& e)
    {
       std::cout << "Program crashed: " << e.what() << '\n';
       return EXIT_FAILURE;
    }
}
