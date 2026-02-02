#pragma once

#include "bgfxTools.h"

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
    
    std::string output = "";
    std::string error = "";
    bool result = false;

    switch (tool)
    {
        case bgfxTool::shaderc:
        {
            bgfxTools::ShaderOptions options;
            options.inputFilePath = "./vs_cubes.sc";
            options.outputFilePath = "abcd.bin";
            options.shaderProfile = bgfxTools::ShaderOptions::Profile::HLSL_4_0;
            options.includePaths.push_back("./");
            options.preprocessorDefines.push_back("FLOAT_ONE=1.0");

            result = bgfxTools::CompileShader(options, &output, &error);

            break;
        }
        case bgfxTool::geometryc:
        {
            break;
        }
        case bgfxTool::texturec:
        {
            break;
        }
        default: break;
    }

    std::cout << "Result: " << (result ? "true" : "false") << "\n";
    if (output.size() > 0)
        std::cout << "---- Output ----\n" << indent(output, 4) << "\n";
    else
        std::cout << "---- No Output ----\n";

    if (error.size() > 0)
        std::cout << "---- Error ----\n" << indent(error, 4) << "\n";

    return EXIT_SUCCESS;
}
