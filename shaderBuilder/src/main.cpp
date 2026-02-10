#include <iostream>

#include "shaderBuilder.h"

int main(int argc, char** argv)
{
    try
    {
       int result = shaderBuilder();
       std::cout << (result == EXIT_SUCCESS ? "Success!\n" : "Failure!\n");
       return result;
    }
    catch(const std::exception& e)
    {
       std::cout << "Program crashed: " << e.what() << '\n';
       return EXIT_FAILURE;
    }
}
