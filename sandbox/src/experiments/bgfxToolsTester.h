#pragma once

#include "bgfxTools.h"
#include "platform/platform.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

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

int bgfxToolTest()
{
    std::string output = "";
    std::string error = "";
    bool result = false;

    {
        std::cout << "Calling CompileShader...\n";

        bgfxTools::ShaderOptions options;
        options.version = true;

        result = bgfxTools::CompileShader(options, &output, &error);

        std::cout << "Result: " << (result ? "true" : "false") << "\n";
        if (output.size() > 0)
            std::cout << "---- Output ----\n" << indent(output, 4) << "\n";
        else
            std::cout << "---- No Output ----\n";
        if (error.size() > 0)
            std::cout << "---- Error ----\n" << indent(error, 4) << "\n";
        std::cout << "\n";
    }

    output = "";
    error = "";

    {
        std::cout << "Calling CompileGeometry...\n";

        bgfxTools::GeometryOptions options;
        options.version = true;

        result = bgfxTools::CompileGeometry(options, &output, &error);

        std::cout << "Result: " << (result ? "true" : "false") << "\n";
        if (output.size() > 0)
            std::cout << "---- Output ----\n" << indent(output, 4) << "\n";
        else
            std::cout << "---- No Output ----\n";
        if (error.size() > 0)
            std::cout << "---- Error ----\n" << indent(error, 4) << "\n";
        std::cout << "\n";
    }

    output = "";
    error = "";

    {
        std::cout << "Calling CompileTexture...\n";

        bgfxTools::TextureOptions options;
        options.version = true;

        result = bgfxTools::CompileTexture(options, &output, &error);

        std::cout << "Result: " << (result ? "true" : "false") << "\n";
        if (output.size() > 0)
            std::cout << "---- Output ----\n" << indent(output, 4) << "\n";
        else
            std::cout << "---- No Output ----\n";
        if (error.size() > 0)
            std::cout << "---- Error ----\n" << indent(error, 4) << "\n";
        std::cout << "\n";
    }

    return EXIT_SUCCESS;
}
