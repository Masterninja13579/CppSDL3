#pragma once

#include <bimg/bimg.h>

#include <string>
#include <vector>

namespace bgfxTools
{
    /// <summary>
    /// Options for the CompileShader function
    /// </summary>
    struct ShaderOptions
    {
        enum class Platform
        {
            Auto = -1,
            Android = 0,
            AsmJS = 1,
            IOS = 2,
            Linux = 3,
            Orbis = 4,
            OSX = 5,
            Windows = 6
        };

        enum class Profile
        {
            ESSL_100 = 0,       // OpenGL ES Shading Language / WebGL (ESSL)
            ESSL_300 = 1,
            ESSL_310 = 2,
            ESSL_320 = 3,

            HLSL_4_0 = 4,       // High-Level Shading Language (HLSL)
            HLSL_5_0 = 5,

            MSL = 6,            // Metal Shading Language (MSL)
            MSL_10_10 = 7,
            MSL_11_10 = 8,
            MSL_12_10 = 9,
            MSL_20_11 = 10,
            MSL_21_11 = 11,
            MSL_22_11 = 12,
            MSL_23_14 = 13,
            MSL_24_14 = 14,
            MSL_30_14 = 15,
            MSL_31_14 = 16,

            PSSL = 17,          // PlayStation Shader Language (PSSL)

            SPIRV = 18,         // Standard Portable Intermediate Representation - V (SPIR-V)
            SPIRV_10_10 = 19,
            SPIRV_13_11 = 20,
            SPIRV_14_11 = 21,
            SPIRV_15_12 = 22,
            SPIRV_16_13 = 23,

            GLSL_120 = 24,      // OpenGL Shading Language (GLSL)
            GLSL_130 = 25,
            GLSL_140 = 26,
            GLSL_150 = 27,
            GLSL_330 = 28,
            GLSL_400 = 29,
            GLSL_410 = 30,
            GLSL_420 = 31,
            GLSL_430 = 32,
            GLSL_440 = 33,

            WGSL = 34           // WGSL
        };

        enum class Type
        {
            None = -1,
            Vertex = 0,
            Fragment = 1,
            Compute = 2
        };

        enum class DXOptimizeLevel
        {
            None = -1,
            L0 = 0,
            L1 = 1,
            L2 = 2,
            L3 = 3
        };

        /// <summary>
        /// Output version information and exit.
        /// </summary>
        bool version = false;

        /// <summary>
        /// Input's file path.
        /// </summary>
        std::string inputFilePath = "";

        /// <summary>
        /// Include path.
        /// </summary>
        std::vector<std::string> includePaths;

        /// <summary>
        /// Output's file path.
        /// </summary>
        std::string outputFilePath = "";

        /// <summary>
        /// Write result to output.
        /// </summary>
        bool writeOutput = false;

        /// <summary>
        /// Generate C header file.  If cHeaderArrayName is not specified base file name will be used as name.
        /// </summary>
        bool generateCHeader = false;

        /// <summary>
        /// C header file array name.  Only used if generateCHeader is true.
        /// </summary>
        std::string cHeaderArrayName = "";

        /// <summary>
        /// Generate makefile style depends file.
        /// </summary>
        bool generateDependsFile = false;

        /// <summary>
        /// Target platform.
        /// </summary>
        Platform targetPlatform = Platform::Auto;

        /// <summary>
        /// Shader model.  Defaults to GLSL.
        /// </summary>
        Profile shaderProfile = Profile::GLSL_330;

        /// <summary>
        /// Only pre-process.
        /// </summary>
        bool preprocess = false;

        /// <summary>
        /// Do not discard comments.
        /// </summary>
        bool keepComments = false;

        /// <summary>
        /// Add defines to preprocessor.
        /// </summary>
        std::vector<std::string> preprocessorDefines;

        /// <summary>
        /// Do not process hsader.  No preprocessor, and no glsl-optimizer.  (GLSL only)
        /// </summary>
        bool raw = false;

        /// <summary>
        /// Shader type.
        /// </summary>
        Type type = Type::Vertex;

        /// <summary>
        /// Set a varying.def.sc's file path
        /// </summary>
        std::string varyingFilePath = "";

        /// <summary>
        /// Be verbose.
        /// </summary>
        bool verbose = false;

        /// <summary>
        /// Debug information.  (Vulkan, DirectX, and Metal)
        /// </summary>
        bool debug = false;

        /// <summary>
        /// Disassemble compiled shader.  (DirectX only)
        /// </summary>
        bool disassemble = false;

        /// <summary>
        /// Set optimization level.  (DirectX only)
        /// </summary>
        DXOptimizeLevel optimize = DXOptimizeLevel::None;

        /// <summary>
        /// Treat warnings as errors.  (DirectX only)
        /// </summary>
        bool warnAsError = false;
    };


    /// <summary>
    /// Options for CompileGeometry function.
    /// </summary>
    struct GeometryOptions
    {
        enum class Coordinates
        {
            LeftHandedYUp = 0,
            LeftHandedZUp = 1,
            RightHandedYUp = 2,
            RightHandedZUp = 3
        };

        /// <summary>
        /// Output version information and exit.
        /// </summary>
        bool version = false;

        /// <summary>
        /// Input's file path.
        /// </summary>
        std::string inputFilePath = "";

        /// <summary>
        /// Output's file path.
        /// </summary>
        std::string outputFilePath = "";

        /// <summary>
        /// Scale factor.
        /// </summary>
        float scale = 1.0f;

        /// <summary>
        /// Front face is counter-clockwise winding order
        /// </summary>
        bool windCounterClockwise = false;

        /// <summary>
        /// Flip texture coordinate V.
        /// </summary>
        bool flipTextureV = false;

        /// <summary>
        /// Number of steps for calculating oriented bounding box.
        /// Defaults to 17.
        /// Less steps = less precise OBB.
        /// More steps = slower calculation.
        /// </summary>
        unsigned short obbSteps = 17;

        /// <summary>
        /// Normal packing.
        /// false - unpacked 12 bytes. (default)
        /// true - packed 4 bytes.
        /// </summary>
        bool packNormals = false;

        /// <summary>
        /// Texture coordinate packing.
        /// false - unpacked 8 bytes. (default)
        /// true - packed 4 bytes.
        /// </summary>
        bool packUVs = false;

        /// <summary>
        /// Calculate tangent vectors. (packing mode is the same as normal)
        /// </summary>
        bool calcTangents = false;

        /// <summary>
        /// Adds barycentric vertex attribute. (Packed in bgfx::Attrib::Color1)
        /// </summary>
        bool barycentric = false;

        /// <summary>
        /// Compress indices.
        /// </summary>
        bool compressIndices = false;

        /// <summary>
        /// Coordinate system.  Defaults to Left-Handed +Y is up.
        /// </summary>
        Coordinates coordinates = Coordinates::LeftHandedYUp;
    };

    /// <summary>
    /// Options for CompileTexture function.
    /// </summary>
    struct TextureOptions
    {
        enum class Quality
        {
            Default,
            Fastest,
            Highest
        };

        enum class LightingModel
        {
            None = -1,
            Phong = 0,
            PhongBrdf = 1,
            Blinn = 2,
            BlinnBrdf = 3,
            GGX = 4
        };

        enum class Extension
        {
            None = -1,
            KTX = 0,
            DDS = 1,
            PNG = 2,
            EXR = 3,
            HDR = 4
        };

        /// <summary>
        /// Output version information and exit.
        /// </summary>
        bool version = false;

        /// <summary>
        /// Input file path.
        /// </summary>
        std::string inputFilePath = "";

        /// <summary>
        /// Output file path.
        /// </summary>
        std::string outputFilePath = "";

        /// <summary>
        /// Output format type (BC1/2/3/4/5, ETC1, PVR14, etc.).
        /// </summary>
        bimg::TextureFormat::Enum outputFormat = bimg::TextureFormat::Unknown;

        /// <summary>
        /// Encoding quality.
        /// </summary>
        Quality encodingQuality = Quality::Default;

        /// <summary>
        /// Generate mip-maps.
        /// </summary>
        bool generateMipMaps = false;

        /// <summary>
        /// Skip N number of mips.
        /// </summary>
        short mipSkips = -1;

        /// <summary>
        /// Input texture is normal map. (Implies linear color space)
        /// </summary>
        bool isNormalMap = false;

        /// <summary>
        /// Input texture is equirectangular projection of cubemap.
        /// </summary>
        bool isEquirectangular = false;

        /// <summary>
        /// Input texture is horizontal or vertical strip of cubemap.
        /// </summary>
        bool isStrip = false;

        /// <summary>
        /// Compute SDF texture.
        /// </summary>
        bool computeSDF = false;

        /// <summary>
        /// Alpha refernce value.
        /// </summary>
        float alphaReference = 0.0f;

        /// <summary>
        /// Image Quality Assesment
        /// </summary>
        bool doQualityAssesment = false;

        /// <summary>
        /// Premultiply alpha into RGB channel
        /// </summary>
        bool premultiplyAlpha = false;

        /// <summary>
        /// Input and output texture is linear color space (gamma correction won't be applied).
        /// </summary>
        bool isLinear = false;

        /// <summary>
        /// Maximum width/height (image will be scaled down and aspect ratio will be preserved)
        /// </summary>
        unsigned int maxSize = 0;

        /// <summary>
        /// Radiance cubemap filter. (Lighting model: Phong, PhongBrdf, Blinn, BlinnBrdf, GGX)
        /// </summary>
        LightingModel radiance = LightingModel::None;

        /// <summary>
        /// Save as.
        /// </summary>
        Extension saveAs = Extension::None;

        /// <summary>
        /// List all supported formats.
        /// </summary>
        bool listFormats = false;

        /// <summary>
        /// DEBUG Validate that output image produced matches after loading.
        /// </summary>
        bool validate = false;
    };
    
    void SetToolDirectoryPath(
        const std::string& path,
        bool relative = true
    );

    bool CompileShaderFromString(
        const std::string& source,
        ShaderOptions& options,
        std::string* output = nullptr,
        std::string* error = nullptr,
        bool writeCommonLibraries = true
    );

    bool CompileShader(
        ShaderOptions& options,
        std::string* output = nullptr,
        std::string* error = nullptr
    );

    bool CompileGeometry(
        GeometryOptions& options,
        std::string* output = nullptr,
        std::string* error = nullptr
    );

    bool CompileTexture(
        TextureOptions& options,
        std::string* output = nullptr,
        std::string* error = nullptr
    );
}
