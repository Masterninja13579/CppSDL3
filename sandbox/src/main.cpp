
#include "experiments/windowTest.h"
#include "experiments/stateMachineTests.h"
#include "experiments/bgfxToolsTester.h"
#include "bgfxExamples/all.h"

#include <iostream>

int main(int argc, char** argv)
{
    try
    {
       int result = bgfxToolTest();
       std::cout << (result == EXIT_SUCCESS ? "Success!\n" : "Failure!\n");
       return result;
    }
    catch(const std::exception& e)
    {
       std::cout << "Program crashed: " << e.what() << '\n';
       return EXIT_FAILURE;
    }
}
