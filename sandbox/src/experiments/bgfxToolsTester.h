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
    
    std::string output = "";
    std::string error = "";
    bool result = bgfx::tools::shaderc(arguments, &output, &error);
    if (!result)
    {
        std::cout << "Error: problem with subprocess\n";
        return EXIT_SUCCESS;
    }

    if (output.size() > 0)
        std::cout << "\n---- Output ----\n" << output << "\n";
    if (error.size() > 0)
        std::cout << "\n---- Error ----\n" << error << "\n";

    return EXIT_SUCCESS;
}
