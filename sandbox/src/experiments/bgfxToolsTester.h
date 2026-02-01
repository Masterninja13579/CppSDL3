#pragma once

#include "bgfxTools/bgfxTools.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

enum bgfxTool
{
    shaderc,
    geometryc,
    texturec
};

namespace
{
    std::string indent(const std::string& text, int indent)
    {
        if (text.size() == 0)
            return "";

        std::string indentString(indent, ' ');
        std::stringstream ss;

        bool doIndent = true;
        for (char c : text)
        {
            if (doIndent)
            {
                ss << indentString;
                doIndent = false;
            }
            ss << c;
            if (c == '\n')
                doIndent = true;
        }

        return ss.str();
    }
}

int bgfxToolTest(bgfxTool tool)
{
    std::string program;
    switch (tool)
    {
        case bgfxTool::shaderc:     program = "shaderc";    break;
        case bgfxTool::geometryc:   program = "geometryc";  break;
        case bgfxTool::texturec:    program = "texturec";   break;
        default: throw std::logic_error("Missing tool program");
    }

    std::cout << "Testing bgfx tool '" << program << "'...\n";

    std::vector<std::string> arguments = {"--help"};
    std::cout << "Arguments:\n";
    for (std::string& arg : arguments)
        std::cout << "    '" << arg << "'\n";
    
    std::string output = "";
    std::string error = "";
    bool result = false;
    if (tool == bgfxTool::shaderc)
        result = bgfx::tools::shaderc(arguments, &output, &error);
    if (tool == bgfxTool::geometryc)
        result = bgfx::tools::geometryc(arguments, &output, &error);
    if (tool == bgfxTool::texturec)
        result = bgfx::tools::texturec(arguments, &output, &error);
    if (!result)
    {
        std::cout << "Error: problem with subprocess\n";
        return EXIT_SUCCESS;
    }

    if (output.size() > 0)
        std::cout << "\n---- Output ----\n" << indent(output, 4) << "\n";
    if (error.size() > 0)
        std::cout << "\n---- Error ----\n" << indent(error, 4) << "\n";

    return EXIT_SUCCESS;
}
