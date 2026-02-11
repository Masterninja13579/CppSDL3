
#include "experiments/bgfxToolsTester.h"
#include "experiments/embedExampleShaders.h"
#include "experiments/stateMachineTests.h"
#include "experiments/windowTest.h"

#include "bgfxExamples/all.h"

#include <iostream>

int main(int argc, char** argv)
{
    try
    {
       int result = bgfxMetaballs();
       std::cout << (result == EXIT_SUCCESS ? "Success!\n" : "Failure!\n");
       return result;
    }
    catch(const std::exception& e)
    {
       std::cout << "Program crashed: " << e.what() << '\n';
       return EXIT_FAILURE;
    }
}
