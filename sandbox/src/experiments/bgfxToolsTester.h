#pragma once

#include "bgfxTools/bgfxTools.h"

#include <iostream>

int bgfxShadercTest()
{
    std::cout << "Testing bgfx tool 'shaderc'...\n";

    std::vector<std::string> arguments = {"--help"};
    std::cout << "Arguments:\n";
    for (std::string& arg : arguments)
        std::cout << "    '" << arg << "'\n";
    
    std::string output;
    std::string error;
    bool result = bgfx::tools::shaderc(arguments, &output, &error);
    if (!result)
    {
        std::cout << "Error: problem with subprocess\n";
        return EXIT_SUCCESS;
    }

    std::cout << "\n---- Output ----\n" << output;
    std::cout << "\n---- Error ----\n" << error;

    return EXIT_SUCCESS;
}
