#pragma once

#include "defines.h"

#include <string>
#include <unordered_map>

const std::unordered_map<std::string, std::string> SHADER_SOURCE_DEFAULT = 
{
    {
        SHADER_TYPE_VERTEX,
        "$input a_position\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));\n"
        "}\n"
    },
    {
        SHADER_TYPE_FRAGMENT,
        "void main()\n"
        "{\n"
        "    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);"
        "}\n"
    },
    {
        SHADER_TYPE_COMPUTE,
        ""
    }
};
const std::unordered_map<std::string, std::string> SHADER_VARYING_DEFAULT =
{
    {
        SHADER_TYPE_VERTEX,
        "vec3 a_position : POSITION;\n"
    },
    {
        SHADER_TYPE_FRAGMENT,
        ""
    },
    {
        SHADER_TYPE_COMPUTE,
        ""
    }
};

