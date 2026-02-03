#include "bgfxTools.h"

#if defined(WIN32) || defined(_WIN32) || defined(__Win32__) || defined(__NT__)
#define OS_WINDOWS 1
#define SUFFIX ".exe"
#elif __APPLE__
#define OS_MAC 1
#define SUFFIX ""
#elif __linux__
#define OS_LINUX 1
#define SUFFIX ""
#else
#error "Unsupported Operating System"
#endif

#include <subprocess.hpp>

#include <functional>
#include <sstream>

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
    const std::string GEOMETRYOPTIONS_COORDINATE[] =
    {
        "-lh-up+y",
        "-lh-up+z",
        "-rh-up+y",
        "-rh-up+z"
    };
    const std::string TEXTUREOPTIONS_QUALITY[] =
    {
        "default",
        "fastest",
        "highest"
    };
    const std::string TEXTUREOPTIONS_LIGHTINGMODEL[] =
    {
        "phong",
        "phongbrdf",
        "blinn",
        "blinnbrdf",
        "ggx"
    };
    const std::string TEXTUREOPTIONS_EXTENSIONS[] =
    {
        ".ktx",
        ".dds",
        ".png",
        ".exr",
        ".hdr"
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

        arguments.push_back(g_toolPath + "shaderc" + SUFFIX);

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

        if (options.varyingFilePath.size() > 0)
        {
            arguments.push_back("--varyingdef");
            arguments.push_back(options.varyingFilePath);
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
        std::vector<std::string> arguments;

        arguments.push_back(g_toolPath + "geometryc" + SUFFIX);

        if (options.version)
            arguments.push_back("--version");

        if (options.inputFilePath.size() > 0)
        {
            arguments.push_back("-f");
            arguments.push_back(options.inputFilePath);
        }

        if (options.outputFilePath.size() > 0)
        {
            arguments.push_back("-o");
            arguments.push_back(options.outputFilePath);
        }

        if (options.scale != 1.0f)
        {
            std::stringstream ss;
            ss << options.scale;
            arguments.push_back("--scale");
            arguments.push_back(ss.str());
        }

        if (options.windCounterClockwise)
            arguments.push_back("--ccw");

        if (options.flipTextureV)
            arguments.push_back("-flipv");

        if (options.obbSteps != 17)
        {
            std::stringstream ss;
            ss << options.obbSteps;
            arguments.push_back("-obb");
            arguments.push_back(ss.str());
        }

        if (options.packNormals)
        {
            arguments.push_back("--packnormal");
            arguments.push_back("1");
        }

        if (options.packUVs)
        {
            arguments.push_back("--packuv");
            arguments.push_back("1");
        }

        if (options.calcTangents)
            arguments.push_back("--tangent");

        if (options.barycentric)
            arguments.push_back("--barycentric");

        if (options.compressIndices)
            arguments.push_back("--compress");

        arguments.push_back(GEOMETRYOPTIONS_COORDINATE[(int)options.coordinates]);

        int result = runProgram(arguments, output, error);

        return result == 0;
    }

    bool CompileTexture(
        TextureOptions& options,
        std::string* output,
        std::string* error)
    {
        std::vector<std::string> arguments;

        arguments.push_back(g_toolPath + "texturec" + SUFFIX);

        if (options.version)
            arguments.push_back("--version");

        if (options.inputFilePath.size() > 0)
        {
            arguments.push_back("-f");
            arguments.push_back(options.inputFilePath);
        }

        if (options.outputFilePath.size() > 0)
        {
            arguments.push_back("-o");
            arguments.push_back(options.outputFilePath);
        }

        if (options.outputFormat != bimg::TextureFormat::Unknown)
        {
            const char* format = bimg::getName(options.outputFormat);
            arguments.push_back("-t");
            arguments.push_back(format);
        }

        if (options.encodingQuality != TextureOptions::Quality::Default)
        {
            arguments.push_back("-q");
            arguments.push_back(TEXTUREOPTIONS_QUALITY[(int)options.encodingQuality]);
        }

        if (options.generateMipMaps)
            arguments.push_back("--mips");

        if (options.mipSkips >= 0)
        {
            std::stringstream ss;
            ss << options.mipSkips;
            arguments.push_back("-mipskip");
            arguments.push_back(ss.str());
        }

        if (options.isNormalMap)
            arguments.push_back("--normalmap");

        if (options.isEquirectangular)
            arguments.push_back("--equirect");

        if (options.isStrip)
            arguments.push_back("--strip");

        if (options.computeSDF)
            arguments.push_back("--sdf");

        if (options.alphaReference > 0.0f)
        {
            std::stringstream ss;
            ss << options.alphaReference;
            arguments.push_back("--ref");
            arguments.push_back(ss.str());
        }

        if (options.doQualityAssesment)
            arguments.push_back("--iqa");

        if (options.premultiplyAlpha)
            arguments.push_back("--pma");

        if (options.isLinear)
            arguments.push_back("--linear");

        if (options.maxSize > 0)
        {
            std::stringstream ss;
            ss << options.maxSize;
            arguments.push_back("--max");
            arguments.push_back(ss.str());
        }

        if (options.radiance != TextureOptions::LightingModel::None)
        {
            arguments.push_back("--radiance");
            arguments.push_back(TEXTUREOPTIONS_LIGHTINGMODEL[(int)options.radiance]);
        }

        if (options.saveAs != TextureOptions::Extension::None)
        {
            arguments.push_back("--as");
            arguments.push_back(TEXTUREOPTIONS_EXTENSIONS[(int)options.saveAs]);
        }

        if (options.listFormats)
            arguments.push_back("--formats");

        if (options.validate)
            arguments.push_back("--validate");

        int result = runProgram(arguments, output, error);

        return result == 0;
    }
}
