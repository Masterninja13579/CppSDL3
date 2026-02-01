#pragma once

#include <string>
#include <vector>

namespace bgfx
{
    namespace tools
    {
        bool shaderc(
            const std::vector<std::string>& arguments,
            std::string* output = nullptr, 
            std::string* error = nullptr
        );

        bool geometryc(
            const std::vector<std::string>& arguments,
            std::string* output = nullptr,
            std::string* error = nullptr
        );

        bool texturec(
            const std::vector<std::string>& arguments,
            std::string* output = nullptr,
            std::string* error = nullptr
        );
    }
}
