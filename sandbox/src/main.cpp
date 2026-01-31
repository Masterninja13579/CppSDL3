
//#include "bgfxExamples/all.h"
//#include "experiments/bgfxTest.h"
//#include "experiments/bgfxToolsTester.h"

#include <iostream>

int tempFunction()
{
    std::cout << "Hello World!\n";
    return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
    try
    {
        int result = tempFunction();
        std::cout << (result == EXIT_SUCCESS ? "Success\n" : "Failed\n");
        return result;
    }
    catch(const std::exception& e)
    {
        std::cout << "Program crashed: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
}
