#include "bgfxTools.h"

#if defined(WIN32) || defined(_WIN32) || defined(__Win32__) || defined(__NT__)
#define OS_WINDOWS
#define SUFFIX ".exe"
#elif __APPLE__
#define OS_MAC
#define SUFFIX ""
#elif __linux__
#define OS_LINUX
#define SUFFIX ""
#else
#error "Unsupported Operating System"
#endif

#include <subprocess.hpp>

// #include <iostream>
#include <functional>
#include <sstream>
// #include <stdexcept>

namespace
{
    std::string g_toolPath = "./";

    const std::string SHADEROPTIONS_PLATFORM[] =
    {
        "android",
        "asm.js",
        "ios",
        "linux",
        "orbis",
        "osx",
        "windows"
    };
    const std::string SHADEROPTIONS_PROFILE[] =
    {
        "100_es",       // OpenGL ES Shading Language / WebGL (ESSL)
        "300_es",
        "310_es",
        "320_es",

        "s_4_0",        // High-Level Shading Language (HLSL)
        "s_5_0",

        "metal",        // Metal Shading Language (MSL)
        "metal10-10",
        "metal11-10",
        "metal12-10",
        "metal20-11",
        "metal21-11",
        "metal22-11",
        "metal23-14",
        "metal24-14",
        "metal30-14",
        "metal31-14",

        "pssl",         // PlayStation Shader Language (PSSL)

        "spirv",        // Standard Portable Intermediate Representation - V (SPIR-V)
        "spirv10-10",
        "spirv13-11",
        "spirv14-11",
        "spirv15-12",
        "spirv16-13",

        "120",          // OpenGL Shading Language (GLSL)
        "130",
        "140",
        "150",
        "330",
        "400",
        "410",
        "420",
        "430",
        "440",

        "wgsl"          // WGSL
    };
    const std::string SHADEROPTIONS_TYPE[] =
    {
        "vertex",
        "fragment",
        "compute"
    };

    int runProgram(
        std::vector<std::string>& arguments,
        std::string* output,
        std::string* error)
    {
        using subprocess::CompletedProcess;
        using subprocess::RunBuilder;
        using subprocess::PipeOption;

        CompletedProcess process = subprocess::run(
            arguments,
            RunBuilder()
                .cout(PipeOption::pipe)
                .cerr(PipeOption::pipe)
        );

        if (output)
            *output = process.cout;
        if (error)
            *error = process.cerr;

        return process.returncode;
    }
}

namespace bgfxTools
{
    void SetToolDirectoryPath(const std::string& path)
    {
        g_toolPath = path;
    }

    bool CompileShader(
        ShaderOptions& options,
        std::string* output,
        std::string* error)
    {
        std::vector<std::string> arguments;

        arguments.push_back(g_toolPath + "shaderc");

        if (options.version)
            arguments.push_back("--version");

        if (options.inputFilePath.size() > 0)
        {
            arguments.push_back("-f");
            arguments.push_back(options.inputFilePath);
        }

        if (options.includePaths.size() > 0)
        {
            for (std::string& path : options.includePaths)
            {
                arguments.push_back("-i");
                arguments.push_back(path);
            }
        }

        if (options.outputFilePath.size() > 0)
        {
            arguments.push_back("-o");
            arguments.push_back(options.outputFilePath);
        }

        if (options.writeOutput)
            arguments.push_back("--stdout");

        if (options.generateCHeader && options.cHeaderArrayName.size() > 0)
        {
            arguments.push_back("--bin2c");
            arguments.push_back(options.cHeaderArrayName);
        }

        if (options.generateDependsFile)
            arguments.push_back("--depends");

        if (options.targetPlatform == ShaderOptions::Platform::Auto)
#ifdef OS_WINDOWS
            options.targetPlatform = ShaderOptions::Platform::Windows;
#elif OS_MAC
            options.targetPlatform = ShaderOptions::Platform::OSX;
#elif OS_LINUX
            options.targetPlatform = ShaderOptions::Platform::Linux;
#endif

        arguments.push_back("--platform");
        arguments.push_back(SHADEROPTIONS_PLATFORM[(int)options.targetPlatform]);

        arguments.push_back("--profile");
        arguments.push_back(SHADEROPTIONS_PROFILE[(int)options.shaderProfile]);

        if (options.preprocess)
            arguments.push_back("--preprocess");

        if (options.keepComments)
            arguments.push_back("--keepcomments");

        if (options.preprocessorDefines.size() > 0)
        {
            std::stringstream ss;
            for (int i = 0; i < options.preprocessorDefines.size(); ++i)
            {
                if (i > 0)
                    ss << ";";
                ss << options.preprocessorDefines[i];
            }
            arguments.push_back("--define");
            arguments.push_back(ss.str());
        }

        if (options.raw)
            arguments.push_back("--raw");

        if (options.type != ShaderOptions::Type::None)
        {
            arguments.push_back("--type");
            arguments.push_back(SHADEROPTIONS_TYPE[(int)options.type]);
        }

        if (options.varyingDefPath.size() > 0)
        {
            arguments.push_back("--varyingdef");
            arguments.push_back(options.varyingDefPath);
        }

        if (options.verbose)
            arguments.push_back("--verbose");

        if (options.debug)
            arguments.push_back("--debug");

        if (options.disassemble)
            arguments.push_back("--disasm");

        if (options.optimize != ShaderOptions::DXOptimizeLevel::None)
        {
            arguments.push_back("-O");
            arguments.push_back(std::to_string((int)options.optimize));
        }

        if (options.warnAsError)
            arguments.push_back("--Werror");

        int result = runProgram(arguments, output, error);

        return result == 0;
    }

    bool CompileGeometry(
        GeometryOptions& options,
        std::string* output,
        std::string* error)
    {
        return true;
    }

    bool CompileTexture(
        TextureOptions& options,
        std::string* output,
        std::string* error)
    {
        return true;
    }
}
