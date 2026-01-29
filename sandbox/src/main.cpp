
#include "bgfxExamples/all.h"
#include "experiments/bgfxTest.h"

#include <iostream>

int main(int argc, char** argv)
{
    try
    {
        int result = bgfxHelloWorld();
        std::cout << (result == EXIT_SUCCESS ? "Success\n" : "Failed\n");
        return result;
    }
    catch(const std::exception& e)
    {
        std::cout << "Program crashed: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
}
