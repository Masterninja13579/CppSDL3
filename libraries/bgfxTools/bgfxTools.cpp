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

#include <filesystem>
#include <fstream>
#include <functional>
#include <mutex>
#include <sstream>

// Shader Common Files
namespace
{
    static const char* FILE_COMMON_NAME = "common.sh";
    static const char* FILE_COMMON_CONTENT = /* Generated with bin2c. */
        "/*\n"
        " * Copyright 2011-2026 Branimir Karadzic. All rights reserved.\n"
        " * License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE\n"
        " */\n"
        "\n"
        "#include <bgfx_shader.sh>\n"
        "#include \"shaderlib.sh\"\n"
        "";
    static const int FILE_COMMON_SIZE = strlen(FILE_COMMON_CONTENT);
    static const char* FILE_BGFXSHADER_NAME = "bgfx_shader.sh";
    static const char* FILE_BGFXSHADER_CONTENT = /* Generated with bin2c. */
        "/*\n"
        " * Copyright 2011-2026 Branimir Karadzic. All rights reserved.\n"
        " * License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE\n"
        " */\n"
        "\n"
        "#ifndef BGFX_SHADER_H_HEADER_GUARD\n"
        "#define BGFX_SHADER_H_HEADER_GUARD\n"
        "\n"
        "#if !defined(BGFX_CONFIG_MAX_BONES)\n"
        "#	define BGFX_CONFIG_MAX_BONES 1\n"
        "#endif // !defined(BGFX_CONFIG_MAX_BONES)\n"
        "\n"
        "#ifndef __cplusplus\n"
        "\n"
        "#if BGFX_SHADER_LANGUAGE_GLSL\n"
        "#	define BRANCH\n"
        "#	define LOOP\n"
        "#	define UNROLL\n"
        "#else\n"
        "#	define BRANCH [branch]\n"
        "#	define LOOP   [loop]\n"
        "#	define UNROLL [unroll]\n"
        "#endif // BGFX_SHADER_LANGUAGE_GLSL\n"
        "\n"
        "#define BGFX_SHADER_MATRIX_COLUMN_MAJOR (0 \\"
        "	|| BGFX_SHADER_LANGUAGE_GLSL           \\"
        "	|| BGFX_SHADER_LANGUAGE_WGSL           \\"
        "	)\n"
        "\n"
        "#if BGFX_SHADER_TYPE_FRAGMENT\n"
        "#	if BGFX_SHADER_LANGUAGE_HLSL  \\"
        "	|| BGFX_SHADER_LANGUAGE_METAL \\"
        "	|| BGFX_SHADER_LANGUAGE_SPIRV \\"
        "	|| BGFX_SHADER_LANGUAGE_WGSL\n"
        "#		define EARLY_DEPTH_STENCIL [earlydepthstencil]\n"
        "	#else\n"
        "#		define EARLY_DEPTH_STENCIL\n"
        "#	endif // BGFX_SHADER_LANGUAGE_...\n"
        "#endif // BGFX_SHADER_TYPE_FRAGMENT\n"
        "\n"
        "#if BGFX_SHADER_LANGUAGE_GLSL\n"
        "#	define ARRAY_BEGIN(_type, _name, _count) _type _name[_count] = _type[](\n"
        "#	define ARRAY_END() )\n"
        "#else\n"
        "#	define ARRAY_BEGIN(_type, _name, _count) _type _name[_count] = {\n"
        "#	define ARRAY_END() }\n"
        "#endif // BGFX_SHADER_LANGUAGE_GLSL\n"
        "\n"
        "#if BGFX_SHADER_LANGUAGE_HLSL  \\"
        " || BGFX_SHADER_LANGUAGE_PSSL  \\"
        " || BGFX_SHADER_LANGUAGE_SPIRV \\"
        " || BGFX_SHADER_LANGUAGE_METAL \\"
        " || BGFX_SHADER_LANGUAGE_WGSL\n"
        "#	define CONST(_x) static const _x\n"
        "#	define dFdx(_x) ddx(_x)\n"
        "#	define dFdy(_y) ddy(-(_y))\n"
        "#	define inversesqrt(_x) rsqrt(_x)\n"
        "#	define fract(_x) frac(_x)\n"
        "\n"
        "#	define bvec2 bool2\n"
        "#	define bvec3 bool3\n"
        "#	define bvec4 bool4\n"
        "\n"
        "// To be able to patch the uav registers on the DXBC SPDB Chunk (D3D11 renderer) the whitespaces around\n"
        "// '_type[_reg]' are necessary. This only affects shaders with debug info (i.e., those that have the SPDB Chunk).\n"
        "#	if BGFX_SHADER_LANGUAGE_HLSL > 400                     \\"
        "	|| BGFX_SHADER_LANGUAGE_PSSL                           \\"
        "	|| BGFX_SHADER_LANGUAGE_SPIRV                          \\"
        "	|| BGFX_SHADER_LANGUAGE_METAL                          \\"
        "	|| BGFX_SHADER_LANGUAGE_WGSL\n"
        "#		define REGISTER(_type, _reg) register( _type[_reg] )\n"
        "#	else\n"
        "#		define REGISTER(_type, _reg) register(_type ##     _reg)\n"
        "#	endif // BGFX_SHADER_LANGUAGE_*\n"
        "\n"
        "#	if BGFX_SHADER_LANGUAGE_HLSL > 400 \\"
        "	|| BGFX_SHADER_LANGUAGE_PSSL       \\"
        "	|| BGFX_SHADER_LANGUAGE_SPIRV      \\"
        "	|| BGFX_SHADER_LANGUAGE_METAL      \\"
        "	|| BGFX_SHADER_LANGUAGE_WGSL\n"
        "#		define dFdxCoarse(_x) ddx_coarse(_x)\n"
        "#		define dFdxFine(_x)   ddx_fine(_x)\n"
        "#		define dFdyCoarse(_y) ddy_coarse(-(_y))\n"
        "#		define dFdyFine(_y)   ddy_fine(-(_y))\n"
        "\n"
        "#		if BGFX_SHADER_LANGUAGE_HLSL  \\"
        "		|| BGFX_SHADER_LANGUAGE_SPIRV \\"
        "		|| BGFX_SHADER_LANGUAGE_METAL \\"
        "		|| BGFX_SHADER_LANGUAGE_WGSL\n"
        "float intBitsToFloat(int   _x) { return asfloat(_x); }\n"
        "vec2  intBitsToFloat(uint2 _x) { return asfloat(_x); }\n"
        "vec3  intBitsToFloat(uint3 _x) { return asfloat(_x); }\n"
        "vec4  intBitsToFloat(uint4 _x) { return asfloat(_x); }\n"
        "#		endif // BGFX_SHADER_LANGUAGE_*\n"
        "\n"
        "float uintBitsToFloat(uint  _x) { return asfloat(_x); }\n"
        "vec2  uintBitsToFloat(uint2 _x) { return asfloat(_x); }\n"
        "vec3  uintBitsToFloat(uint3 _x) { return asfloat(_x); }\n"
        "vec4  uintBitsToFloat(uint4 _x) { return asfloat(_x); }\n"
        "\n"
        "uint  floatBitsToUint(float _x) { return asuint(_x); }\n"
        "uvec2 floatBitsToUint(vec2  _x) { return asuint(_x); }\n"
        "uvec3 floatBitsToUint(vec3  _x) { return asuint(_x); }\n"
        "uvec4 floatBitsToUint(vec4  _x) { return asuint(_x); }\n"
        "\n"
        "int   floatBitsToInt(float _x) { return asint(_x); }\n"
        "ivec2 floatBitsToInt(vec2  _x) { return asint(_x); }\n"
        "ivec3 floatBitsToInt(vec3  _x) { return asint(_x); }\n"
        "ivec4 floatBitsToInt(vec4  _x) { return asint(_x); }\n"
        "\n"
        "uint  bitfieldReverse(uint  _x) { return reversebits(_x); }\n"
        "uint2 bitfieldReverse(uint2 _x) { return reversebits(_x); }\n"
        "uint3 bitfieldReverse(uint3 _x) { return reversebits(_x); }\n"
        "uint4 bitfieldReverse(uint4 _x) { return reversebits(_x); }\n"
        "\n"
        "#		if !BGFX_SHADER_LANGUAGE_SPIRV\n"
        "uint packHalf2x16(vec2 _x)\n"
        "{\n"
        "	return (f32tof16(_x.y)<<16) | f32tof16(_x.x);\n"
        "}\n"
        "\n"
        "vec2 unpackHalf2x16(uint _x)\n"
        "{\n"
        "	return vec2(f16tof32(_x & 0xffff), f16tof32(_x >> 16) );\n"
        "}\n"
        "#		endif // !BGFX_SHADER_LANGUAGE_SPIRV\n"
        "\n"
        "struct BgfxSampler2D\n"
        "{\n"
        "	SamplerState m_sampler;\n"
        "	Texture2D m_texture;\n"
        "};\n"
        "\n"
        "struct BgfxISampler2D\n"
        "{\n"
        "	Texture2D<ivec4> m_texture;\n"
        "};\n"
        "\n"
        "struct BgfxUSampler2D\n"
        "{\n"
        "	Texture2D<uvec4> m_texture;\n"
        "};\n"
        "\n"
        "struct BgfxSampler2DArray\n"
        "{\n"
        "	SamplerState m_sampler;\n"
        "	Texture2DArray m_texture;\n"
        "};\n"
        "\n"
        "struct BgfxSampler2DShadow\n"
        "{\n"
        "	SamplerComparisonState m_sampler;\n"
        "	Texture2D m_texture;\n"
        "};\n"
        "\n"
        "struct BgfxSampler2DArrayShadow\n"
        "{\n"
        "	SamplerComparisonState m_sampler;\n"
        "	Texture2DArray m_texture;\n"
        "};\n"
        "\n"
        "struct BgfxSampler3D\n"
        "{\n"
        "	SamplerState m_sampler;\n"
        "	Texture3D m_texture;\n"
        "};\n"
        "\n"
        "struct BgfxISampler3D\n"
        "{\n"
        "	Texture3D<ivec4> m_texture;\n"
        "};\n"
        "\n"
        "struct BgfxUSampler3D\n"
        "{\n"
        "	Texture3D<uvec4> m_texture;\n"
        "};\n"
        "\n"
        "struct BgfxSamplerCube\n"
        "{\n"
        "	SamplerState m_sampler;\n"
        "	TextureCube m_texture;\n"
        "};\n"
        "\n"
        "struct BgfxSamplerCubeShadow\n"
        "{\n"
        "	SamplerComparisonState m_sampler;\n"
        "	TextureCube m_texture;\n"
        "};\n"
        "\n"
        "struct BgfxSampler2DMS\n"
        "{\n"
        "	Texture2DMS<vec4> m_texture;\n"
        "};\n"
        "\n"
        "vec4 bgfxTexture2D(BgfxSampler2D _sampler, vec2 _coord)\n"
        "{\n"
        "	return _sampler.m_texture.Sample(_sampler.m_sampler, _coord);\n"
        "}\n"
        "\n"
        "vec4 bgfxTexture2DBias(BgfxSampler2D _sampler, vec2 _coord, float _bias)\n"
        "{\n"
        "	return _sampler.m_texture.SampleBias(_sampler.m_sampler, _coord, _bias);\n"
        "}\n"
        "\n"
        "vec4 bgfxTexture2DLod(BgfxSampler2D _sampler, vec2 _coord, float _level)\n"
        "{\n"
        "	return _sampler.m_texture.SampleLevel(_sampler.m_sampler, _coord, _level);\n"
        "}\n"
        "\n"
        "vec4 bgfxTexture2DLodOffset(BgfxSampler2D _sampler, vec2 _coord, float _level, ivec2 _offset)\n"
        "{\n"
        "	return _sampler.m_texture.SampleLevel(_sampler.m_sampler, _coord, _level, _offset);\n"
        "}\n"
        "\n"
        "vec4 bgfxTexture2DProj(BgfxSampler2D _sampler, vec3 _coord)\n"
        "{\n"
        "	vec2 coord = _coord.xy * rcp(_coord.z);\n"
        "	return _sampler.m_texture.Sample(_sampler.m_sampler, coord);\n"
        "}\n"
        "\n"
        "vec4 bgfxTexture2DProj(BgfxSampler2D _sampler, vec4 _coord)\n"
        "{\n"
        "	vec2 coord = _coord.xy * rcp(_coord.w);\n"
        "	return _sampler.m_texture.Sample(_sampler.m_sampler, coord);\n"
        "}\n"
        "\n"
        "vec4 bgfxTexture2DGrad(BgfxSampler2D _sampler, vec2 _coord, vec2 _dPdx, vec2 _dPdy)\n"
        "{\n"
        "	return _sampler.m_texture.SampleGrad(_sampler.m_sampler, _coord, _dPdx, _dPdy);\n"
        "}\n"
        "\n"
        "vec4 bgfxTexture2DArray(BgfxSampler2DArray _sampler, vec3 _coord)\n"
        "{\n"
        "	return _sampler.m_texture.Sample(_sampler.m_sampler, _coord);\n"
        "}\n"
        "\n"
        "vec4 bgfxTexture2DArrayLod(BgfxSampler2DArray _sampler, vec3 _coord, float _lod)\n"
        "{\n"
        "	return _sampler.m_texture.SampleLevel(_sampler.m_sampler, _coord, _lod);\n"
        "}\n"
        "\n"
        "vec4 bgfxTexture2DArrayLodOffset(BgfxSampler2DArray _sampler, vec3 _coord, float _level, ivec2 _offset)\n"
        "{\n"
        "	return _sampler.m_texture.SampleLevel(_sampler.m_sampler, _coord, _level, _offset);\n"
        "}\n"
        "\n"
        "float bgfxShadow2D(BgfxSampler2DShadow _sampler, vec3 _coord)\n"
        "{\n"
        "	return _sampler.m_texture.SampleCmpLevelZero(_sampler.m_sampler, _coord.xy, _coord.z);\n"
        "}\n"
        "\n"
        "float bgfxShadow2DProj(BgfxSampler2DShadow _sampler, vec4 _coord)\n"
        "{\n"
        "	vec3 coord = _coord.xyz * rcp(_coord.w);\n"
        "	return _sampler.m_texture.SampleCmpLevelZero(_sampler.m_sampler, coord.xy, coord.z);\n"
        "}\n"
        "\n"
        "vec2 bgfxTextureSize(BgfxSampler2DShadow _sampler, int _lod)\n"
        "{\n"
        "	vec2 result;\n"
        "	float numberOfMipMapLevels;\n"
        "	_sampler.m_texture.GetDimensions(_lod, result.x, result.y, numberOfMipMapLevels);\n"
        "	return result;\n"
        "}\n"
        "\n"
        "vec4 bgfxShadow2DArray(BgfxSampler2DArrayShadow _sampler, vec4 _coord)\n"
        "{\n"
        "	return _sampler.m_texture.SampleCmpLevelZero(_sampler.m_sampler, _coord.xyz, _coord.w);\n"
        "}\n"
        "\n"
        "vec2 bgfxTextureSize(BgfxSampler2DArrayShadow _sampler, int _lod)\n"
        "{\n"
        "	vec2 result;\n"
        "	float numberOfMipMapLevels;\n"
        "	float numberOfElements;\n"
        "	_sampler.m_texture.GetDimensions(_lod, result.x, result.y, numberOfElements, numberOfMipMapLevels);\n"
        "	return result;\n"
        "}\n"
        "\n"
        "vec4 bgfxTexture3D(BgfxSampler3D _sampler, vec3 _coord)\n"
        "{\n"
        "	return _sampler.m_texture.Sample(_sampler.m_sampler, _coord);\n"
        "}\n"
        "\n"
        "vec4 bgfxTexture3DLod(BgfxSampler3D _sampler, vec3 _coord, float _level)\n"
        "{\n"
        "	return _sampler.m_texture.SampleLevel(_sampler.m_sampler, _coord, _level);\n"
        "}\n"
        "\n"
        "ivec4 bgfxTexture3D(BgfxISampler3D _sampler, vec3 _coord)\n"
        "{\n"
        "	uvec3 size;\n"
        "	_sampler.m_texture.GetDimensions(size.x, size.y, size.z);\n"
        "	return _sampler.m_texture.Load(ivec4(_coord * size, 0) );\n"
        "}\n"
        "\n"
        "uvec4 bgfxTexture3D(BgfxUSampler3D _sampler, vec3 _coord)\n"
        "{\n"
        "	uvec3 size;\n"
        "	_sampler.m_texture.GetDimensions(size.x, size.y, size.z);\n"
        "	return _sampler.m_texture.Load(ivec4(_coord * size, 0) );\n"
        "}\n"
        "\n"
        "vec4 bgfxTextureCube(BgfxSamplerCube _sampler, vec3 _coord)\n"
        "{\n"
        "	return _sampler.m_texture.Sample(_sampler.m_sampler, _coord);\n"
        "}\n"
        "\n"
        "vec4 bgfxTextureCubeBias(BgfxSamplerCube _sampler, vec3 _coord, float _bias)\n"
        "{\n"
        "	return _sampler.m_texture.SampleBias(_sampler.m_sampler, _coord, _bias);\n"
        "}\n"
        "\n"
        "vec4 bgfxTextureCubeLod(BgfxSamplerCube _sampler, vec3 _coord, float _level)\n"
        "{\n"
        "	return _sampler.m_texture.SampleLevel(_sampler.m_sampler, _coord, _level);\n"
        "}\n"
        "\n"
        "float bgfxShadowCube(BgfxSamplerCubeShadow _sampler, vec4 _coord)\n"
        "{\n"
        "	return _sampler.m_texture.SampleCmpLevelZero(_sampler.m_sampler, _coord.xyz, _coord.w);\n"
        "}\n"
        "\n"
        "vec4 bgfxTexelFetch(BgfxSampler2D _sampler, ivec2 _coord, int _lod)\n"
        "{\n"
        "	return _sampler.m_texture.Load(ivec3(_coord, _lod) );\n"
        "}\n"
        "\n"
        "vec4 bgfxTexelFetchOffset(BgfxSampler2D _sampler, ivec2 _coord, int _lod, ivec2 _offset)\n"
        "{\n"
        "	return _sampler.m_texture.Load(ivec3(_coord, _lod), _offset );\n"
        "}\n"
        "\n"
        "vec2 bgfxTextureSize(BgfxSampler2D _sampler, int _lod)\n"
        "{\n"
        "	vec2 result;\n"
        "	float numberOfMipMapLevels;\n"
        "	_sampler.m_texture.GetDimensions(_lod, result.x, result.y, numberOfMipMapLevels);\n"
        "	return result;\n"
        "}\n"
        "\n"
        "vec2 bgfxTextureSize(BgfxISampler2D _sampler, int _lod)\n"
        "{\n"
        "	vec2 result;\n"
        "	float numberOfMipMapLevels;\n"
        "	_sampler.m_texture.GetDimensions(_lod, result.x, result.y, numberOfMipMapLevels);\n"
        "	return result;\n"
        "}\n"
        "\n"
        "vec2 bgfxTextureSize(BgfxUSampler2D _sampler, int _lod)\n"
        "{\n"
        "	vec2 result;\n"
        "	float numberOfMipMapLevels;\n"
        "	_sampler.m_texture.GetDimensions(_lod, result.x, result.y, numberOfMipMapLevels);\n"
        "	return result;\n"
        "}\n"
        "\n"
        "vec4 bgfxTextureGather0(BgfxSampler2D _sampler, vec2 _coord)\n"
        "{\n"
        "	return _sampler.m_texture.GatherRed(_sampler.m_sampler, _coord);\n"
        "}\n"
        "\n"
        "vec4 bgfxTextureGather1(BgfxSampler2D _sampler, vec2 _coord)\n"
        "{\n"
        "	return _sampler.m_texture.GatherGreen(_sampler.m_sampler, _coord);\n"
        "}\n"
        "\n"
        "vec4 bgfxTextureGather2(BgfxSampler2D _sampler, vec2 _coord)\n"
        "{\n"
        "	return _sampler.m_texture.GatherBlue(_sampler.m_sampler, _coord);\n"
        "}\n"
        "\n"
        "vec4 bgfxTextureGather3(BgfxSampler2D _sampler, vec2 _coord)\n"
        "{\n"
        "	return _sampler.m_texture.GatherAlpha(_sampler.m_sampler, _coord);\n"
        "}\n"
        "\n"
        "vec4 bgfxTextureGatherOffset0(BgfxSampler2D _sampler, vec2 _coord, ivec2 _offset)\n"
        "{\n"
        "	return _sampler.m_texture.GatherRed(_sampler.m_sampler, _coord, _offset);\n"
        "}\n"
        "\n"
        "vec4 bgfxTextureGatherOffset1(BgfxSampler2D _sampler, vec2 _coord, ivec2 _offset)\n"
        "{\n"
        "	return _sampler.m_texture.GatherGreen(_sampler.m_sampler, _coord, _offset);\n"
        "}\n"
        "\n"
        "vec4 bgfxTextureGatherOffset2(BgfxSampler2D _sampler, vec2 _coord, ivec2 _offset)\n"
        "{\n"
        "	return _sampler.m_texture.GatherBlue(_sampler.m_sampler, _coord, _offset);\n"
        "}\n"
        "\n"
        "vec4 bgfxTextureGatherOffset3(BgfxSampler2D _sampler, vec2 _coord, ivec2 _offset)\n"
        "{\n"
        "	return _sampler.m_texture.GatherAlpha(_sampler.m_sampler, _coord, _offset);\n"
        "}\n"
        "\n"
        "vec4 bgfxTextureGather0(BgfxSampler2DArray _sampler, vec3 _coord)\n"
        "{\n"
        "	return _sampler.m_texture.GatherRed(_sampler.m_sampler, _coord);\n"
        "}\n"
        "\n"
        "vec4 bgfxTextureGather1(BgfxSampler2DArray _sampler, vec3 _coord)\n"
        "{\n"
        "	return _sampler.m_texture.GatherGreen(_sampler.m_sampler, _coord);\n"
        "}\n"
        "\n"
        "vec4 bgfxTextureGather2(BgfxSampler2DArray _sampler, vec3 _coord)\n"
        "{\n"
        "	return _sampler.m_texture.GatherBlue(_sampler.m_sampler, _coord);\n"
        "}\n"
        "\n"
        "vec4 bgfxTextureGather3(BgfxSampler2DArray _sampler, vec3 _coord)\n"
        "{\n"
        "	return _sampler.m_texture.GatherAlpha(_sampler.m_sampler, _coord);\n"
        "}\n"
        "\n"
        "ivec4 bgfxTexelFetch(BgfxISampler2D _sampler, ivec2 _coord, int _lod)\n"
        "{\n"
        "	return _sampler.m_texture.Load(ivec3(_coord, _lod) );\n"
        "}\n"
        "\n"
        "uvec4 bgfxTexelFetch(BgfxUSampler2D _sampler, ivec2 _coord, int _lod)\n"
        "{\n"
        "	return _sampler.m_texture.Load(ivec3(_coord, _lod) );\n"
        "}\n"
        "\n"
        "vec4 bgfxTexelFetch(BgfxSampler2DMS _sampler, ivec2 _coord, int _sampleIdx)\n"
        "{\n"
        "	return _sampler.m_texture.Load(_coord, _sampleIdx);\n"
        "}\n"
        "\n"
        "vec4 bgfxTexelFetch(BgfxSampler2DArray _sampler, ivec3 _coord, int _lod)\n"
        "{\n"
        "	return _sampler.m_texture.Load(ivec4(_coord, _lod) );\n"
        "}\n"
        "\n"
        "vec4 bgfxTexelFetch(BgfxSampler3D _sampler, ivec3 _coord, int _lod)\n"
        "{\n"
        "	return _sampler.m_texture.Load(ivec4(_coord, _lod) );\n"
        "}\n"
        "\n"
        "vec3 bgfxTextureSize(BgfxSampler3D _sampler, int _lod)\n"
        "{\n"
        "	vec3 result;\n"
        "	float numberOfMipMapLevels;\n"
        "	_sampler.m_texture.GetDimensions(_lod, result.x, result.y, result.z, numberOfMipMapLevels);\n"
        "	return result;\n"
        "}\n"
        "\n"
        "#		define SAMPLER2D(_name, _reg) \\"
        "			uniform SamplerState _name ## Sampler : REGISTER(s, _reg); \\"
        "			uniform Texture2D _name ## Texture : REGISTER(t, _reg); \\"
        "			static BgfxSampler2D _name = { _name ## Sampler, _name ## Texture }\n"
        "#		define ISAMPLER2D(_name, _reg) \\"
        "			uniform Texture2D<ivec4> _name ## Texture : REGISTER(t, _reg); \\"
        "			static BgfxISampler2D _name = { _name ## Texture }\n"
        "#		define USAMPLER2D(_name, _reg) \\"
        "			uniform Texture2D<uvec4> _name ## Texture : REGISTER(t, _reg); \\"
        "			static BgfxUSampler2D _name = { _name ## Texture }\n"
        "#		define sampler2D BgfxSampler2D\n"
        "#		define texture2D(_sampler, _coord) bgfxTexture2D(_sampler, _coord)\n"
        "#		define texture2DBias(_sampler, _coord, _bias) bgfxTexture2DBias(_sampler, _coord, _bias)\n"
        "#		define texture2DLod(_sampler, _coord, _level) bgfxTexture2DLod(_sampler, _coord, _level)\n"
        "#		define texture2DLodOffset(_sampler, _coord, _level, _offset) bgfxTexture2DLodOffset(_sampler, _coord, _level, _offset)\n"
        "#		define texture2DProj(_sampler, _coord) bgfxTexture2DProj(_sampler, _coord)\n"
        "#		define texture2DGrad(_sampler, _coord, _dPdx, _dPdy) bgfxTexture2DGrad(_sampler, _coord, _dPdx, _dPdy)\n"
        "\n"
        "#		define SAMPLER2DARRAY(_name, _reg) \\"
        "			uniform SamplerState _name ## Sampler : REGISTER(s, _reg); \\"
        "			uniform Texture2DArray _name ## Texture : REGISTER(t, _reg); \\"
        "			static BgfxSampler2DArray _name = { _name ## Sampler, _name ## Texture }\n"
        "#		define sampler2DArray BgfxSampler2DArray\n"
        "#		define texture2DArray(_sampler, _coord) bgfxTexture2DArray(_sampler, _coord)\n"
        "#		define texture2DArrayLod(_sampler, _coord, _lod) bgfxTexture2DArrayLod(_sampler, _coord, _lod)\n"
        "#		define texture2DArrayLodOffset(_sampler, _coord, _level, _offset) bgfxTexture2DArrayLodOffset(_sampler, _coord, _level, _offset)\n"
        "\n"
        "#		define SAMPLER2DMS(_name, _reg) \\"
        "			uniform Texture2DMS<vec4> _name ## Texture : REGISTER(t, _reg); \\"
        "			static BgfxSampler2DMS _name = { _name ## Texture }\n"
        "#		define sampler2DMS BgfxSampler2DMS\n"
        "\n"
        "#		define SAMPLER2DSHADOW(_name, _reg) \\"
        "			uniform SamplerComparisonState _name ## SamplerComparison : REGISTER(s, _reg); \\"
        "			uniform Texture2D _name ## Texture : REGISTER(t, _reg); \\"
        "			static BgfxSampler2DShadow _name = { _name ## SamplerComparison, _name ## Texture }\n"
        "#		define sampler2DShadow BgfxSampler2DShadow\n"
        "#		define shadow2D(_sampler, _coord) bgfxShadow2D(_sampler, _coord)\n"
        "#		define shadow2DProj(_sampler, _coord) bgfxShadow2DProj(_sampler, _coord)\n"
        "\n"
        "#		define SAMPLER2DARRAYSHADOW(_name, _reg) \\"
        "			uniform SamplerComparisonState _name ## SamplerComparison : REGISTER(s, _reg); \\"
        "			uniform Texture2DArray _name ## Texture : REGISTER(t, _reg); \\"
        "			static BgfxSampler2DArrayShadow _name = { _name ## SamplerComparison, _name ## Texture }\n"
        "#		define sampler2DArrayShadow BgfxSampler2DArrayShadow\n"
        "#		define shadow2DArray(_sampler, _coord) bgfxShadow2DArray(_sampler, _coord)\n"
        "\n"
        "#		define SAMPLER3D(_name, _reg) \\"
        "			uniform SamplerState _name ## Sampler : REGISTER(s, _reg); \\"
        "			uniform Texture3D _name ## Texture : REGISTER(t, _reg); \\"
        "			static BgfxSampler3D _name = { _name ## Sampler, _name ## Texture }\n"
        "#		define ISAMPLER3D(_name, _reg) \\"
        "			uniform Texture3D<ivec4> _name ## Texture : REGISTER(t, _reg); \\"
        "			static BgfxISampler3D _name = { _name ## Texture }\n"
        "#		define USAMPLER3D(_name, _reg) \\"
        "			uniform Texture3D<uvec4> _name ## Texture : REGISTER(t, _reg); \\"
        "			static BgfxUSampler3D _name = { _name ## Texture }\n"
        "#		define sampler3D BgfxSampler3D\n"
        "#		define texture3D(_sampler, _coord) bgfxTexture3D(_sampler, _coord)\n"
        "#		define texture3DLod(_sampler, _coord, _level) bgfxTexture3DLod(_sampler, _coord, _level)\n"
        "\n"
        "#		define SAMPLERCUBE(_name, _reg) \\"
        "			uniform SamplerState _name ## Sampler : REGISTER(s, _reg); \\"
        "			uniform TextureCube _name ## Texture : REGISTER(t, _reg); \\"
        "			static BgfxSamplerCube _name = { _name ## Sampler, _name ## Texture }\n"
        "#		define samplerCube BgfxSamplerCube\n"
        "#		define textureCube(_sampler, _coord) bgfxTextureCube(_sampler, _coord)\n"
        "#		define textureCubeBias(_sampler, _coord, _bias) bgfxTextureCubeBias(_sampler, _coord, _bias)\n"
        "#		define textureCubeLod(_sampler, _coord, _level) bgfxTextureCubeLod(_sampler, _coord, _level)\n"
        "\n"
        "#		define SAMPLERCUBESHADOW(_name, _reg) \\"
        "			uniform SamplerComparisonState _name ## SamplerComparison : REGISTER(s, _reg); \\"
        "			uniform TextureCube _name ## Texture : REGISTER(t, _reg); \\"
        "			static BgfxSamplerCubeShadow _name = { _name ## SamplerComparison, _name ## Texture }\n"
        "#		define samplerCubeShadow BgfxSamplerCubeShadow\n"
        "#		define shadowCube(_sampler, _coord) bgfxShadowCube(_sampler, _coord)\n"
        "\n"
        "#		define texelFetch(_sampler, _coord, _lod) bgfxTexelFetch(_sampler, _coord, _lod)\n"
        "#		define texelFetchOffset(_sampler, _coord, _lod, _offset) bgfxTexelFetchOffset(_sampler, _coord, _lod, _offset)\n"
        "#		define textureSize(_sampler, _lod) bgfxTextureSize(_sampler, _lod)\n"
        "#		define textureGather(_sampler, _coord, _comp) bgfxTextureGather ## _comp(_sampler, _coord)\n"
        "#		define textureGatherOffset(_sampler, _coord, _offset, _comp) bgfxTextureGatherOffset ## _comp(_sampler, _coord, _offset)\n"
        "#	else\n"
        "\n"
        "#		define sampler2DShadow sampler2D\n"
        "\n"
        "vec4 bgfxTexture2DProj(sampler2D _sampler, vec3 _coord)\n"
        "{\n"
        "	return tex2Dproj(_sampler, vec4(_coord.xy, 0.0, _coord.z) );\n"
        "}\n"
        "\n"
        "vec4 bgfxTexture2DProj(sampler2D _sampler, vec4 _coord)\n"
        "{\n"
        "	return tex2Dproj(_sampler, _coord);\n"
        "}\n"
        "\n"
        "float bgfxShadow2D(sampler2DShadow _sampler, vec3 _coord)\n"
        "{\n"
        "#if 0\n"
        "	float occluder = tex2D(_sampler, _coord.xy).x;\n"
        "	return step(_coord.z, occluder);\n"
        "#else\n"
        "	return tex2Dproj(_sampler, vec4(_coord.xy, _coord.z, 1.0) ).x;\n"
        "#endif // 0\n"
        "}\n"
        "\n"
        "float bgfxShadow2DProj(sampler2DShadow _sampler, vec4 _coord)\n"
        "{\n"
        "#if 0\n"
        "	vec3 coord = _coord.xyz * rcp(_coord.w);\n"
        "	float occluder = tex2D(_sampler, coord.xy).x;\n"
        "	return step(coord.z, occluder);\n"
        "#else\n"
        "	return tex2Dproj(_sampler, _coord).x;\n"
        "#endif // 0\n"
        "}\n"
        "\n"
        "#		define SAMPLER2D(_name, _reg) uniform sampler2D _name : REGISTER(s, _reg)\n"
        "#		define SAMPLER2DMS(_name, _reg) uniform sampler2DMS _name : REGISTER(s, _reg)\n"
        "#		define texture2D(_sampler, _coord) tex2D(_sampler, _coord)\n"
        "#		define texture2DProj(_sampler, _coord) bgfxTexture2DProj(_sampler, _coord)\n"
        "\n"
        "#		define SAMPLER2DARRAY(_name, _reg) SAMPLER2D(_name, _reg)\n"
        "#		define texture2DArray(_sampler, _coord) texture2D(_sampler, (_coord).xy)\n"
        "#		define texture2DArrayLod(_sampler, _coord, _lod) texture2DLod(_sampler, _coord, _lod)\n"
        "\n"
        "#		define SAMPLER2DSHADOW(_name, _reg) uniform sampler2DShadow _name : REGISTER(s, _reg)\n"
        "#		define shadow2D(_sampler, _coord) bgfxShadow2D(_sampler, _coord)\n"
        "#		define shadow2DProj(_sampler, _coord) bgfxShadow2DProj(_sampler, _coord)\n"
        "\n"
        "#		define SAMPLER3D(_name, _reg) uniform sampler3D _name : REGISTER(s, _reg)\n"
        "#		define texture3D(_sampler, _coord) tex3D(_sampler, _coord)\n"
        "\n"
        "#		define SAMPLERCUBE(_name, _reg) uniform samplerCUBE _name : REGISTER(s, _reg)\n"
        "#		define textureCube(_sampler, _coord) texCUBE(_sampler, _coord)\n"
        "\n"
        "#		define texture2DLod(_sampler, _coord, _level) tex2Dlod(_sampler, vec4( (_coord).xy, 0.0, _level) )\n"
        "#		define texture2DGrad(_sampler, _coord, _dPdx, _dPdy) tex2Dgrad(_sampler, _coord, _dPdx, _dPdy)\n"
        "#		define texture3DLod(_sampler, _coord, _level) tex3Dlod(_sampler, vec4( (_coord).xyz, _level) )\n"
        "#		define textureCubeLod(_sampler, _coord, _level) texCUBElod(_sampler, vec4( (_coord).xyz, _level) )\n"
        "\n"
        "#	endif // BGFX_SHADER_LANGUAGE_HLSL > 300\n"
        "\n"
        "bvec2 lessThan(vec2 _a, vec2 _b) { return _a < _b; }\n"
        "bvec3 lessThan(vec3 _a, vec3 _b) { return _a < _b; }\n"
        "bvec4 lessThan(vec4 _a, vec4 _b) { return _a < _b; }\n"
        "\n"
        "bvec2 lessThanEqual(vec2 _a, vec2 _b) { return _a <= _b; }\n"
        "bvec3 lessThanEqual(vec3 _a, vec3 _b) { return _a <= _b; }\n"
        "bvec4 lessThanEqual(vec4 _a, vec4 _b) { return _a <= _b; }\n"
        "\n"
        "bvec2 greaterThan(vec2 _a, vec2 _b) { return _a > _b; }\n"
        "bvec3 greaterThan(vec3 _a, vec3 _b) { return _a > _b; }\n"
        "bvec4 greaterThan(vec4 _a, vec4 _b) { return _a > _b; }\n"
        "\n"
        "bvec2 greaterThanEqual(vec2 _a, vec2 _b) { return _a >= _b; }\n"
        "bvec3 greaterThanEqual(vec3 _a, vec3 _b) { return _a >= _b; }\n"
        "bvec4 greaterThanEqual(vec4 _a, vec4 _b) { return _a >= _b; }\n"
        "\n"
        "bvec2 notEqual(vec2 _a, vec2 _b) { return _a != _b; }\n"
        "bvec3 notEqual(vec3 _a, vec3 _b) { return _a != _b; }\n"
        "bvec4 notEqual(vec4 _a, vec4 _b) { return _a != _b; }\n"
        "\n"
        "bvec2 equal(vec2 _a, vec2 _b) { return _a == _b; }\n"
        "bvec3 equal(vec3 _a, vec3 _b) { return _a == _b; }\n"
        "bvec4 equal(vec4 _a, vec4 _b) { return _a == _b; }\n"
        "\n"
        "float mix(float _a, float _b, float _t) { return lerp(_a, _b, _t); }\n"
        "vec2  mix(vec2  _a, vec2  _b, vec2  _t) { return lerp(_a, _b, _t); }\n"
        "vec3  mix(vec3  _a, vec3  _b, vec3  _t) { return lerp(_a, _b, _t); }\n"
        "vec4  mix(vec4  _a, vec4  _b, vec4  _t) { return lerp(_a, _b, _t); }\n"
        "\n"
        "float mod(float _a, float _b) { return _a - _b * floor(_a / _b); }\n"
        "vec2  mod(vec2  _a, vec2  _b) { return _a - _b * floor(_a / _b); }\n"
        "vec3  mod(vec3  _a, vec3  _b) { return _a - _b * floor(_a / _b); }\n"
        "vec4  mod(vec4  _a, vec4  _b) { return _a - _b * floor(_a / _b); }\n"
        "\n"
        "#else\n"
        "#	define CONST(_x) const _x\n"
        "#	define atan2(_x, _y) atan(_x, _y)\n"
        "#	define saturate(_x) clamp(_x, 0.0, 1.0)\n"
        "#	define SAMPLER2D(_name, _reg)       uniform sampler2D _name\n"
        "#	define SAMPLER2DMS(_name, _reg)     uniform sampler2DMS _name\n"
        "#	define SAMPLER3D(_name, _reg)       uniform sampler3D _name\n"
        "#	define SAMPLERCUBE(_name, _reg)     uniform samplerCube _name\n"
        "#	define SAMPLER2DSHADOW(_name, _reg) uniform sampler2DShadow _name\n"
        "\n"
        "#	define SAMPLER2DARRAY(_name, _reg)       uniform sampler2DArray _name\n"
        "#	define SAMPLER2DMSARRAY(_name, _reg)     uniform sampler2DMSArray _name\n"
        "#	define SAMPLERCUBEARRAY(_name, _reg)     uniform samplerCubeArray _name\n"
        "#	define SAMPLER2DARRAYSHADOW(_name, _reg) uniform sampler2DArrayShadow _name\n"
        "\n"
        "#	define ISAMPLER2D(_name, _reg) uniform isampler2D _name\n"
        "#	define USAMPLER2D(_name, _reg) uniform usampler2D _name\n"
        "#	define ISAMPLER3D(_name, _reg) uniform isampler3D _name\n"
        "#	define USAMPLER3D(_name, _reg) uniform usampler3D _name\n"
        "\n"
        "#	if BGFX_SHADER_LANGUAGE_GLSL >= 130\n"
        "#		define texture2D(_sampler, _coord)      texture(_sampler, _coord)\n"
        "#		define texture2DArray(_sampler, _coord) texture(_sampler, _coord)\n"
        "#		define texture3D(_sampler, _coord)      texture(_sampler, _coord)\n"
        "#		define textureCube(_sampler, _coord)    texture(_sampler, _coord)\n"
        "#		define texture2DLod(_sampler, _coord, _lod)                textureLod(_sampler, _coord, _lod)\n"
        "#		define texture2DLodOffset(_sampler, _coord, _lod, _offset) textureLodOffset(_sampler, _coord, _lod, _offset)\n"
        "#		define texture2DBias(_sampler, _coord, _bias)      texture(_sampler, _coord, _bias)\n"
        "#		define textureCubeBias(_sampler, _coord, _bias)    texture(_sampler, _coord, _bias)\n"
        "#	else\n"
        "#		define texture2DBias(_sampler, _coord, _bias)      texture2D(_sampler, _coord, _bias)\n"
        "#		define textureCubeBias(_sampler, _coord, _bias)    textureCube(_sampler, _coord, _bias)\n"
        "#	endif // BGFX_SHADER_LANGUAGE_GLSL >= 130\n"
        "\n"
        "float rcp(float _a) { return 1.0/_a; }\n"
        "vec2  rcp(vec2  _a) { return vec2(1.0)/_a; }\n"
        "vec3  rcp(vec3  _a) { return vec3(1.0)/_a; }\n"
        "vec4  rcp(vec4  _a) { return vec4(1.0)/_a; }\n"
        "#endif // BGFX_SHADER_LANGUAGE_*\n"
        "\n"
        "vec2 vec2_splat(float _x) { return vec2(_x, _x); }\n"
        "vec3 vec3_splat(float _x) { return vec3(_x, _x, _x); }\n"
        "vec4 vec4_splat(float _x) { return vec4(_x, _x, _x, _x); }\n"
        "\n"
        "#if BGFX_SHADER_LANGUAGE_GLSL >= 130 \\"
        " || BGFX_SHADER_LANGUAGE_HLSL        \\"
        " || BGFX_SHADER_LANGUAGE_PSSL        \\"
        " || BGFX_SHADER_LANGUAGE_SPIRV       \\"
        " || BGFX_SHADER_LANGUAGE_METAL       \\"
        " || BGFX_SHADER_LANGUAGE_WGSL\n"
        "uvec2 uvec2_splat(uint _x) { return uvec2(_x, _x); }\n"
        "uvec3 uvec3_splat(uint _x) { return uvec3(_x, _x, _x); }\n"
        "uvec4 uvec4_splat(uint _x) { return uvec4(_x, _x, _x, _x); }\n"
        "#endif // BGFX_SHADER_LANGUAGE_*\n"
        "\n"
        "#if BGFX_SHADER_LANGUAGE_GLSL\n"
        "#	define mul(_a, _b) ( (_a) * (_b) )\n"
        "#elif BGFX_SHADER_LANGUAGE_WGSL\n"
        "#	define mul(_a, _b) mul(_b, _a)\n"
        "#	define mat3x4 float3x4\n"
        "#	define mat4x3 float4x3\n"
        "#else\n"
        "#	define mul(_a, _b) mul(_a, _b)\n"
        "#	define mat3x4 float4x3\n"
        "#	define mat4x3 float3x4\n"
        "#endif // BGFX_SHADER_LANGUAGE_*\n"
        "\n"
        "mat4 mtxFromRows(vec4 _0, vec4 _1, vec4 _2, vec4 _3)\n"
        "{\n"
        "#if BGFX_SHADER_MATRIX_COLUMN_MAJOR\n"
        "	return transpose(mat4(_0, _1, _2, _3) );\n"
        "#else\n"
        "	return mat4(_0, _1, _2, _3);\n"
        "#endif // BGFX_SHADER_LANGUAGE_GLSL\n"
        "}\n"
        "\n"
        "mat4 mtxFromCols(vec4 _0, vec4 _1, vec4 _2, vec4 _3)\n"
        "{\n"
        "#if BGFX_SHADER_MATRIX_COLUMN_MAJOR\n"
        "	return mat4(_0, _1, _2, _3);\n"
        "#else\n"
        "	return transpose(mat4(_0, _1, _2, _3) );\n"
        "#endif // BGFX_SHADER_LANGUAGE_GLSL\n"
        "}\n"
        "\n"
        "mat3 mtxFromRows(vec3 _0, vec3 _1, vec3 _2)\n"
        "{\n"
        "#if BGFX_SHADER_MATRIX_COLUMN_MAJOR\n"
        "	return transpose(mat3(_0, _1, _2) );\n"
        "#else\n"
        "	return mat3(_0, _1, _2);\n"
        "#endif // BGFX_SHADER_LANGUAGE_GLSL\n"
        "}\n"
        "\n"
        "mat3 mtxFromCols(vec3 _0, vec3 _1, vec3 _2)\n"
        "{\n"
        "#if BGFX_SHADER_MATRIX_COLUMN_MAJOR\n"
        "	return mat3(_0, _1, _2);\n"
        "#else\n"
        "	return transpose(mat3(_0, _1, _2) );\n"
        "#endif // BGFX_SHADER_LANGUAGE_GLSL\n"
        "}\n"
        "\n"
        "#if !BGFX_SHADER_LANGUAGE_ESSL\n"
        "mat4x3 mtxFromRows(vec4 _0, vec4 _1, vec4 _2)\n"
        "{\n"
        "#if BGFX_SHADER_MATRIX_COLUMN_MAJOR\n"
        "    return transpose(mat3x4(_0, _1, _2) );\n"
        "#else\n"
        "	return mat4x3(_0, _1, _2);\n"
        "#endif // BGFX_SHADER_LANGUAGE_GLSL\n"
        "}\n"
        "#endif // !BGFX_SHADER_LANGUAGE_ESSL\n"
        "\n"
        "vec3 mtxGetRow(mat3 _mtx, int _row)\n"
        "{\n"
        "#if BGFX_SHADER_MATRIX_COLUMN_MAJOR\n"
        "    return vec3(_mtx[0][_row], _mtx[1][_row], _mtx[2][_row]);\n"
        "#else\n"
        "    return vec3(_mtx[_row]);\n"
        "#endif // BGFX_SHADER_LANGUAGE_GLSL\n"
        "}\n"
        "\n"
        "vec3 mtxGetColumn(mat3 _mtx, int _column)\n"
        "{\n"
        "#if BGFX_SHADER_MATRIX_COLUMN_MAJOR\n"
        "    return vec3(_mtx[_column]);\n"
        "#else\n"
        "    return vec3(_mtx[0][_column], _mtx[1][_column], _mtx[2][_column]);\n"
        "#endif // BGFX_SHADER_LANGUAGE_GLSL\n"
        "}\n"
        "\n"
        "#if !BGFX_SHADER_LANGUAGE_ESSL\n"
        "vec4 mtxGetRow(mat4x3 _mtx, int _row)\n"
        "{\n"
        "#if BGFX_SHADER_MATRIX_COLUMN_MAJOR\n"
        "    return vec4(_mtx[0][_row], _mtx[1][_row], _mtx[2][_row], _mtx[3][_row]);\n"
        "#else\n"
        "    return vec4(_mtx[_row]);\n"
        "#endif // BGFX_SHADER_LANGUAGE_GLSL\n"
        "}\n"
        "\n"
        "vec3 mtxGetColumn(mat4x3 _mtx, int _column)\n"
        "{\n"
        "#if BGFX_SHADER_MATRIX_COLUMN_MAJOR\n"
        "    return vec3(_mtx[_column]);\n"
        "#else\n"
        "    return vec3(_mtx[0][_column], _mtx[1][_column], _mtx[2][_column]);\n"
        "#endif // BGFX_SHADER_LANGUAGE_GLSL\n"
        "}\n"
        "#endif // !BGFX_SHADER_LANGUAGE_ESSL\n"
        "\n"
        "vec4 mtxGetRow(mat4 _mtx, int _row)\n"
        "{\n"
        "#if BGFX_SHADER_MATRIX_COLUMN_MAJOR\n"
        "    return vec4(_mtx[0][_row], _mtx[1][_row], _mtx[2][_row], _mtx[3][_row]);\n"
        "#else\n"
        "    return vec4(_mtx[_row]);\n"
        "#endif // BGFX_SHADER_LANGUAGE_GLSL\n"
        "}\n"
        "\n"
        "vec4 mtxGetColumn(mat4 _mtx, int _column)\n"
        "{\n"
        "#if BGFX_SHADER_MATRIX_COLUMN_MAJOR\n"
        "    return vec4(_mtx[_column]);\n"
        "#else\n"
        "    return vec4(_mtx[0][_column], _mtx[1][_column], _mtx[2][_column], _mtx[3][_column]);\n"
        "#endif // BGFX_SHADER_LANGUAGE_GLSL\n"
        "}\n"
        "\n"
        "float mtxGetElement(mat3 _mtx, int _column, int _row)\n"
        "{\n"
        "#if BGFX_SHADER_MATRIX_COLUMN_MAJOR\n"
        "    return _mtx[_column][_row];\n"
        "#else\n"
        "    return _mtx[_row][_column];\n"
        "#endif // BGFX_SHADER_LANGUAGE_GLSL\n"
        "}\n"
        "\n"
        "float mtxGetElement(mat4 _mtx, int _column, int _row)\n"
        "{\n"
        "#if BGFX_SHADER_MATRIX_COLUMN_MAJOR\n"
        "    return _mtx[_column][_row];\n"
        "#else\n"
        "    return _mtx[_row][_column];\n"
        "#endif // BGFX_SHADER_LANGUAGE_GLSL\n"
        "}\n"
        "\n"
        "uniform vec4 u_viewRect;\n"
        "uniform vec4 u_viewTexel;\n"
        "uniform mat4 u_view;\n"
        "uniform mat4 u_invView;\n"
        "uniform mat4 u_proj;\n"
        "uniform mat4 u_invProj;\n"
        "uniform mat4 u_viewProj;\n"
        "uniform mat4 u_invViewProj;\n"
        "uniform mat4 u_modelView;\n"
        "uniform mat4 u_invModelView;\n"
        "uniform mat4 u_modelViewProj;\n"
        "uniform vec4 u_alphaRef4;\n"
        "#define u_alphaRef u_alphaRef4.x\n"
        "uniform mat4 u_model[BGFX_CONFIG_MAX_BONES];\n"
        "\n"
        "#endif // __cplusplus\n"
        "\n"
        "#endif // BGFX_SHADER_H_HEADER_GUARD\n"
        "";
    static const int FILE_BGFXSHADER_SIZE = strlen(FILE_BGFXSHADER_CONTENT);
    static const char* FILE_SHADERLIB_NAME = "shaderlib.sh";
    static const char* FILE_SHADERLIB_CONTENT = /* Generated with bin2c. */
        "/*\n"
        " * Copyright 2011-2026 Branimir Karadzic. All rights reserved.\n"
        " * License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE\n"
        " */\n"
        "\n"
        "#ifndef __SHADERLIB_SH__\n"
        "#define __SHADERLIB_SH__\n"
        "\n"
        "vec4 encodeRE8(float _r)\n"
        "{\n"
        "	float exponent = ceil(log2(_r) );\n"
        "	return vec4(_r / exp2(exponent)\n"
        "		, 0.0\n"
        "		, 0.0\n"
        "		, (exponent + 128.0) / 255.0\n"
        "		);\n"
        "}\n"
        "\n"
        "float decodeRE8(vec4 _re8)\n"
        "{\n"
        "	float exponent = _re8.w * 255.0 - 128.0;\n"
        "	return _re8.x * exp2(exponent);\n"
        "}\n"
        "\n"
        "vec4 encodeRGBE8(vec3 _rgb)\n"
        "{\n"
        "	vec4 rgbe8;\n"
        "	float maxComponent = max(max(_rgb.x, _rgb.y), _rgb.z);\n"
        "	float exponent = ceil(log2(maxComponent) );\n"
        "	rgbe8.xyz = _rgb / exp2(exponent);\n"
        "	rgbe8.w = (exponent + 128.0) / 255.0;\n"
        "	return rgbe8;\n"
        "}\n"
        "\n"
        "vec3 decodeRGBE8(vec4 _rgbe8)\n"
        "{\n"
        "	float exponent = _rgbe8.w * 255.0 - 128.0;\n"
        "	vec3 rgb = _rgbe8.xyz * exp2(exponent);\n"
        "	return rgb;\n"
        "}\n"
        "\n"
        "vec3 encodeNormalUint(vec3 _normal)\n"
        "{\n"
        "	return _normal * 0.5 + 0.5;\n"
        "}\n"
        "\n"
        "vec3 decodeNormalUint(vec3 _encodedNormal)\n"
        "{\n"
        "	return _encodedNormal * 2.0 - 1.0;\n"
        "}\n"
        "\n"
        "vec2 encodeNormalSphereMap(vec3 _normal)\n"
        "{\n"
        "	return normalize(_normal.xy) * sqrt(_normal.z * 0.5 + 0.5);\n"
        "}\n"
        "\n"
        "vec3 decodeNormalSphereMap(vec2 _encodedNormal)\n"
        "{\n"
        "	float zz = dot(_encodedNormal, _encodedNormal) * 2.0 - 1.0;\n"
        "	return vec3(normalize(_encodedNormal.xy) * sqrt(1.0 - zz*zz), zz);\n"
        "}\n"
        "\n"
        "vec2 octahedronWrap(vec2 _val)\n"
        "{\n"
        "	// Reference(s):\n"
        "	// - Octahedron normal vector encoding\n"
        "	//   https://web.archive.org/web/20191027010600/https://knarkowicz.wordpress.com/2014/04/16/octahedron-normal-vector-encoding/comment-page-1/\n"
        "	return (1.0 - abs(_val.yx) )\n"
        "		 * mix(vec2_splat(-1.0), vec2_splat(1.0), vec2(greaterThanEqual(_val.xy, vec2_splat(0.0) ) ) );\n"
        "}\n"
        "\n"
        "vec2 encodeNormalOctahedron(vec3 _normal)\n"
        "{\n"
        "	_normal /= abs(_normal.x) + abs(_normal.y) + abs(_normal.z);\n"
        "	_normal.xy = _normal.z >= 0.0 ? _normal.xy : octahedronWrap(_normal.xy);\n"
        "	_normal.xy = _normal.xy * 0.5 + 0.5;\n"
        "	return _normal.xy;\n"
        "}\n"
        "\n"
        "vec3 decodeNormalOctahedron(vec2 _encodedNormal)\n"
        "{\n"
        "	_encodedNormal = _encodedNormal * 2.0 - 1.0;\n"
        "\n"
        "	vec3 normal;\n"
        "	normal.z  = 1.0 - abs(_encodedNormal.x) - abs(_encodedNormal.y);\n"
        "	normal.xy = normal.z >= 0.0 ? _encodedNormal.xy : octahedronWrap(_encodedNormal.xy);\n"
        "	return normalize(normal);\n"
        "}\n"
        "\n"
        "vec3 convertRGB2XYZ(vec3 _rgb)\n"
        "{\n"
        "	// Reference(s):\n"
        "	// - RGB/XYZ Matrices\n"
        "	//   https://web.archive.org/web/20191027010220/http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html\n"
        "	vec3 xyz;\n"
        "	xyz.x = dot(vec3(0.4124564, 0.3575761, 0.1804375), _rgb);\n"
        "	xyz.y = dot(vec3(0.2126729, 0.7151522, 0.0721750), _rgb);\n"
        "	xyz.z = dot(vec3(0.0193339, 0.1191920, 0.9503041), _rgb);\n"
        "	return xyz;\n"
        "}\n"
        "\n"
        "vec3 convertXYZ2RGB(vec3 _xyz)\n"
        "{\n"
        "	vec3 rgb;\n"
        "	rgb.x = dot(vec3( 3.2404542, -1.5371385, -0.4985314), _xyz);\n"
        "	rgb.y = dot(vec3(-0.9692660,  1.8760108,  0.0415560), _xyz);\n"
        "	rgb.z = dot(vec3( 0.0556434, -0.2040259,  1.0572252), _xyz);\n"
        "	return rgb;\n"
        "}\n"
        "\n"
        "vec3 convertXYZ2Yxy(vec3 _xyz)\n"
        "{\n"
        "	// Reference(s):\n"
        "	// - XYZ to xyY\n"
        "	//   https://web.archive.org/web/20191027010144/http://www.brucelindbloom.com/index.html?Eqn_XYZ_to_xyY.html\n"
        "	float inv = 1.0/dot(_xyz, vec3(1.0, 1.0, 1.0) );\n"
        "	return vec3(_xyz.y, _xyz.x*inv, _xyz.y*inv);\n"
        "}\n"
        "\n"
        "vec3 convertYxy2XYZ(vec3 _Yxy)\n"
        "{\n"
        "	// Reference(s):\n"
        "	// - xyY to XYZ\n"
        "	//   https://web.archive.org/web/20191027010036/http://www.brucelindbloom.com/index.html?Eqn_xyY_to_XYZ.html\n"
        "	vec3 xyz;\n"
        "	xyz.x = _Yxy.x*_Yxy.y/_Yxy.z;\n"
        "	xyz.y = _Yxy.x;\n"
        "	xyz.z = _Yxy.x*(1.0 - _Yxy.y - _Yxy.z)/_Yxy.z;\n"
        "	return xyz;\n"
        "}\n"
        "\n"
        "vec3 convertRGB2Yxy(vec3 _rgb)\n"
        "{\n"
        "	return convertXYZ2Yxy(convertRGB2XYZ(_rgb) );\n"
        "}\n"
        "\n"
        "vec3 convertYxy2RGB(vec3 _Yxy)\n"
        "{\n"
        "	return convertXYZ2RGB(convertYxy2XYZ(_Yxy) );\n"
        "}\n"
        "\n"
        "vec3 convertRGB2Yuv(vec3 _rgb)\n"
        "{\n"
        "	vec3 yuv;\n"
        "	yuv.x = dot(_rgb, vec3(0.299, 0.587, 0.114) );\n"
        "	yuv.y = (_rgb.x - yuv.x)*0.713 + 0.5;\n"
        "	yuv.z = (_rgb.z - yuv.x)*0.564 + 0.5;\n"
        "	return yuv;\n"
        "}\n"
        "\n"
        "vec3 convertYuv2RGB(vec3 _yuv)\n"
        "{\n"
        "	vec3 rgb;\n"
        "	rgb.x = _yuv.x + 1.403*(_yuv.y-0.5);\n"
        "	rgb.y = _yuv.x - 0.344*(_yuv.y-0.5) - 0.714*(_yuv.z-0.5);\n"
        "	rgb.z = _yuv.x + 1.773*(_yuv.z-0.5);\n"
        "	return rgb;\n"
        "}\n"
        "\n"
        "vec3 convertRGB2YIQ(vec3 _rgb)\n"
        "{\n"
        "	vec3 yiq;\n"
        "	yiq.x = dot(vec3(0.299,     0.587,     0.114   ), _rgb);\n"
        "	yiq.y = dot(vec3(0.595716, -0.274453, -0.321263), _rgb);\n"
        "	yiq.z = dot(vec3(0.211456, -0.522591,  0.311135), _rgb);\n"
        "	return yiq;\n"
        "}\n"
        "\n"
        "vec3 convertYIQ2RGB(vec3 _yiq)\n"
        "{\n"
        "	vec3 rgb;\n"
        "	rgb.x = dot(vec3(1.0,  0.9563,  0.6210), _yiq);\n"
        "	rgb.y = dot(vec3(1.0, -0.2721, -0.6474), _yiq);\n"
        "	rgb.z = dot(vec3(1.0, -1.1070,  1.7046), _yiq);\n"
        "	return rgb;\n"
        "}\n"
        "\n"
        "vec3 toLinear(vec3 _rgb)\n"
        "{\n"
        "	return pow(abs(_rgb), vec3_splat(2.2) );\n"
        "}\n"
        "\n"
        "vec4 toLinear(vec4 _rgba)\n"
        "{\n"
        "	return vec4(toLinear(_rgba.xyz), _rgba.w);\n"
        "}\n"
        "\n"
        "vec3 toLinearAccurate(vec3 _rgb)\n"
        "{\n"
        "	vec3 lo = _rgb / 12.92;\n"
        "	vec3 hi = pow( (_rgb + 0.055) / 1.055, vec3_splat(2.4) );\n"
        "	vec3 rgb = mix(hi, lo, vec3(lessThanEqual(_rgb, vec3_splat(0.04045) ) ) );\n"
        "	return rgb;\n"
        "}\n"
        "\n"
        "vec4 toLinearAccurate(vec4 _rgba)\n"
        "{\n"
        "	return vec4(toLinearAccurate(_rgba.xyz), _rgba.w);\n"
        "}\n"
        "\n"
        "float toGamma(float _r)\n"
        "{\n"
        "	return pow(abs(_r), 1.0/2.2);\n"
        "}\n"
        "\n"
        "vec3 toGamma(vec3 _rgb)\n"
        "{\n"
        "	return pow(abs(_rgb), vec3_splat(1.0/2.2) );\n"
        "}\n"
        "\n"
        "vec4 toGamma(vec4 _rgba)\n"
        "{\n"
        "	return vec4(toGamma(_rgba.xyz), _rgba.w);\n"
        "}\n"
        "\n"
        "vec3 toGammaAccurate(vec3 _rgb)\n"
        "{\n"
        "	vec3 lo  = _rgb * 12.92;\n"
        "	vec3 hi  = pow(abs(_rgb), vec3_splat(1.0/2.4) ) * 1.055 - 0.055;\n"
        "	vec3 rgb = mix(hi, lo, vec3(lessThanEqual(_rgb, vec3_splat(0.0031308) ) ) );\n"
        "	return rgb;\n"
        "}\n"
        "\n"
        "vec4 toGammaAccurate(vec4 _rgba)\n"
        "{\n"
        "	return vec4(toGammaAccurate(_rgba.xyz), _rgba.w);\n"
        "}\n"
        "\n"
        "vec3 toReinhard(vec3 _rgb)\n"
        "{\n"
        "	return toGamma(_rgb/(_rgb+vec3_splat(1.0) ) );\n"
        "}\n"
        "\n"
        "vec4 toReinhard(vec4 _rgba)\n"
        "{\n"
        "	return vec4(toReinhard(_rgba.xyz), _rgba.w);\n"
        "}\n"
        "\n"
        "vec3 toFilmic(vec3 _rgb)\n"
        "{\n"
        "	_rgb = max(vec3_splat(0.0), _rgb - 0.004);\n"
        "	_rgb = (_rgb*(6.2*_rgb + 0.5) ) / (_rgb*(6.2*_rgb + 1.7) + 0.06);\n"
        "	return _rgb;\n"
        "}\n"
        "\n"
        "vec4 toFilmic(vec4 _rgba)\n"
        "{\n"
        "	return vec4(toFilmic(_rgba.xyz), _rgba.w);\n"
        "}\n"
        "\n"
        "vec3 toAcesFilmic(vec3 _rgb)\n"
        "{\n"
        "	// Reference(s):\n"
        "	// - ACES Filmic Tone Mapping Curve\n"
        "	//   https://web.archive.org/web/20191027010704/https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/\n"
        "	float aa = 2.51f;\n"
        "	float bb = 0.03f;\n"
        "	float cc = 2.43f;\n"
        "	float dd = 0.59f;\n"
        "	float ee = 0.14f;\n"
        "	return saturate( (_rgb*(aa*_rgb + bb) )/(_rgb*(cc*_rgb + dd) + ee) );\n"
        "}\n"
        "\n"
        "vec4 toAcesFilmic(vec4 _rgba)\n"
        "{\n"
        "	return vec4(toAcesFilmic(_rgba.xyz), _rgba.w);\n"
        "}\n"
        "\n"
        "vec3 luma(vec3 _rgb)\n"
        "{\n"
        "	float yy = dot(vec3(0.2126729, 0.7151522, 0.0721750), _rgb);\n"
        "	return vec3_splat(yy);\n"
        "}\n"
        "\n"
        "vec4 luma(vec4 _rgba)\n"
        "{\n"
        "	return vec4(luma(_rgba.xyz), _rgba.w);\n"
        "}\n"
        "\n"
        "vec3 conSatBri(vec3 _rgb, vec3 _csb)\n"
        "{\n"
        "	vec3 rgb = _rgb * _csb.z;\n"
        "	rgb = mix(luma(rgb), rgb, _csb.y);\n"
        "	rgb = mix(vec3_splat(0.5), rgb, _csb.x);\n"
        "	return rgb;\n"
        "}\n"
        "\n"
        "vec4 conSatBri(vec4 _rgba, vec3 _csb)\n"
        "{\n"
        "	return vec4(conSatBri(_rgba.xyz, _csb), _rgba.w);\n"
        "}\n"
        "\n"
        "vec3 posterize(vec3 _rgb, float _numColors)\n"
        "{\n"
        "	return floor(_rgb*_numColors) / _numColors;\n"
        "}\n"
        "\n"
        "vec4 posterize(vec4 _rgba, float _numColors)\n"
        "{\n"
        "	return vec4(posterize(_rgba.xyz, _numColors), _rgba.w);\n"
        "}\n"
        "\n"
        "vec3 sepia(vec3 _rgb)\n"
        "{\n"
        "	vec3 color;\n"
        "	color.x = dot(_rgb, vec3(0.393, 0.769, 0.189) );\n"
        "	color.y = dot(_rgb, vec3(0.349, 0.686, 0.168) );\n"
        "	color.z = dot(_rgb, vec3(0.272, 0.534, 0.131) );\n"
        "	return color;\n"
        "}\n"
        "\n"
        "vec4 sepia(vec4 _rgba)\n"
        "{\n"
        "	return vec4(sepia(_rgba.xyz), _rgba.w);\n"
        "}\n"
        "\n"
        "vec3 blendOverlay(vec3 _base, vec3 _blend)\n"
        "{\n"
        "	vec3 lt = 2.0 * _base * _blend;\n"
        "	vec3 gte = 1.0 - 2.0 * (1.0 - _base) * (1.0 - _blend);\n"
        "	return mix(lt, gte, step(vec3_splat(0.5), _base) );\n"
        "}\n"
        "\n"
        "vec4 blendOverlay(vec4 _base, vec4 _blend)\n"
        "{\n"
        "	return vec4(blendOverlay(_base.xyz, _blend.xyz), _base.w);\n"
        "}\n"
        "\n"
        "vec3 adjustHue(vec3 _rgb, float _hue)\n"
        "{\n"
        "	vec3 yiq = convertRGB2YIQ(_rgb);\n"
        "	float angle = _hue + atan2(yiq.z, yiq.y);\n"
        "	float len = length(yiq.yz);\n"
        "	return convertYIQ2RGB(vec3(yiq.x, len*cos(angle), len*sin(angle) ) );\n"
        "}\n"
        "\n"
        "vec4 packFloatToRgba(float _value)\n"
        "{\n"
        "	// Reference(s):\n"
        "	// - Encoding floats to RGBA - the final?\n"
        "	//   https://web.archive.org/web/20250914131649/https://aras-p.info/blog/2009/07/30/encoding-floats-to-rgba-the-final/\n"
        "	const vec4 shift = vec4(255 * 255 * 255, 255 * 255, 255, 1.0);\n"
        "	const vec4 mask = vec4(0, 1.0 / 255.0, 1.0 / 255.0, 1.0 / 255.0);\n"
        "	vec4 comp = fract(_value * shift);\n"
        "	comp -= comp.xxyz * mask;\n"
        "	return comp;\n"
        "}\n"
        "\n"
        "float unpackRgbaToFloat(vec4 _rgba)\n"
        "{\n"
        "	const vec4 shift = vec4(1.0 / (255.0 * 255.0 * 255.0), 1.0 / (255.0 * 255.0), 1.0 / 255.0, 1.0);\n"
        "	return dot(_rgba, shift);\n"
        "}\n"
        "\n"
        "vec2 packHalfFloat(float _value)\n"
        "{\n"
        "	const vec2 shift = vec2(255, 1.0);\n"
        "	const vec2 mask = vec2(0, 1.0 / 255.0);\n"
        "	vec2 comp = fract(_value * shift);\n"
        "	comp -= comp.xx * mask;\n"
        "	return comp;\n"
        "}\n"
        "\n"
        "float unpackHalfFloat(vec2 _rg)\n"
        "{\n"
        "	const vec2 shift = vec2(1.0 / 255.0, 1.0);\n"
        "	return dot(_rg, shift);\n"
        "}\n"
        "\n"
        "float random(vec2 _uv)\n"
        "{\n"
        "	return fract(sin(dot(_uv.xy, vec2(12.9898, 78.233) ) ) * 43758.5453);\n"
        "}\n"
        "\n"
        "vec3 fixCubeLookup(vec3 _v, float _lod, float _topLevelCubeSize)\n"
        "{\n"
        "	// Reference(s):\n"
        "	// - Seamless cube-map filtering\n"
        "	//   https://web.archive.org/web/20190411181934/http://the-witness.net/news/2012/02/seamless-cube-map-filtering/\n"
        "	float ax = abs(_v.x);\n"
        "	float ay = abs(_v.y);\n"
        "	float az = abs(_v.z);\n"
        "	float vmax = max(max(ax, ay), az);\n"
        "	float scale = 1.0 - exp2(_lod) / _topLevelCubeSize;\n"
        "	if (ax != vmax) { _v.x *= scale; }\n"
        "	if (ay != vmax) { _v.y *= scale; }\n"
        "	if (az != vmax) { _v.z *= scale; }\n"
        "	return _v;\n"
        "}\n"
        "\n"
        "vec2 texture2DBc5(sampler2D _sampler, vec2 _uv)\n"
        "{\n"
        "#if BGFX_SHADER_LANGUAGE_HLSL && BGFX_SHADER_LANGUAGE_HLSL <= 300\n"
        "	return texture2D(_sampler, _uv).yx;\n"
        "#else\n"
        "	return texture2D(_sampler, _uv).xy;\n"
        "#endif\n"
        "}\n"
        "\n"
        "mat3 cofactor(mat4 _m)\n"
        "{\n"
        "	// Reference:\n"
        "	// Cofactor of matrix. Use to transform normals. The code assumes the last column of _m is [0,0,0,1].\n"
        "	// https://www.shadertoy.com/view/3s33zj\n"
        "	// https://github.com/graphitemaster/normals_revisited\n"
        "	return mat3(\n"
        "		_m[1][1]*_m[2][2]-_m[1][2]*_m[2][1],\n"
        "		_m[1][2]*_m[2][0]-_m[1][0]*_m[2][2],\n"
        "		_m[1][0]*_m[2][1]-_m[1][1]*_m[2][0],\n"
        "		_m[0][2]*_m[2][1]-_m[0][1]*_m[2][2],\n"
        "		_m[0][0]*_m[2][2]-_m[0][2]*_m[2][0],\n"
        "		_m[0][1]*_m[2][0]-_m[0][0]*_m[2][1],\n"
        "		_m[0][1]*_m[1][2]-_m[0][2]*_m[1][1],\n"
        "		_m[0][2]*_m[1][0]-_m[0][0]*_m[1][2],\n"
        "		_m[0][0]*_m[1][1]-_m[0][1]*_m[1][0]\n"
        "		);\n"
        "}\n"
        "\n"
        "float toClipSpaceDepth(float _depthTextureZ)\n"
        "{\n"
        "#if BGFX_SHADER_LANGUAGE_GLSL\n"
        "	return _depthTextureZ * 2.0 - 1.0;\n"
        "#else\n"
        "	return _depthTextureZ;\n"
        "#endif // BGFX_SHADER_LANGUAGE_GLSL\n"
        "}\n"
        "\n"
        "vec3 clipToWorld(mat4 _invViewProj, vec3 _clipPos)\n"
        "{\n"
        "	vec4 wpos = mul(_invViewProj, vec4(_clipPos, 1.0) );\n"
        "	return wpos.xyz / wpos.w;\n"
        "}\n"
        "\n"
        "#endif // __SHADERLIB_SH__\n"
        "";
    static const int FILE_SHADERLIB_SIZE = strlen(FILE_SHADERLIB_CONTENT);

    static const std::string SHADEROPTIONS_PLATFORM[] =
    {
        "android",
        "asm.js",
        "ios",
        "linux",
        "orbis",
        "osx",
        "windows"
    };
    static const std::string SHADEROPTIONS_PROFILE[] =
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
    static const std::string SHADEROPTIONS_TYPE[] =
    {
        "vertex",
        "fragment",
        "compute"
    };
    static const std::string GEOMETRYOPTIONS_COORDINATE[] =
    {
        "-lh-up+y",
        "-lh-up+z",
        "-rh-up+y",
        "-rh-up+z"
    };
    static const std::string TEXTUREOPTIONS_QUALITY[] =
    {
        "default",
        "fastest",
        "highest"
    };
    static const std::string TEXTUREOPTIONS_LIGHTINGMODEL[] =
    {
        "phong",
        "phongbrdf",
        "blinn",
        "blinnbrdf",
        "ggx"
    };
    static const std::string TEXTUREOPTIONS_EXTENSIONS[] =
    {
        ".ktx",
        ".dds",
        ".png",
        ".exr",
        ".hdr"
    };
}

// Utilities
namespace
{
    std::string g_toolPath = "./";

    int runProgram(
        std::vector<std::string>& arguments,
        std::string* output,
        std::string* error)
    {
        using subprocess::CompletedProcess;
        using subprocess::RunBuilder;
        using subprocess::PipeOption;

        CompletedProcess process;

        try
        {
            process = subprocess::run(
                arguments,
                RunBuilder()
                    .cout(PipeOption::pipe)
                    .cerr(PipeOption::pipe)
            );
        }
        catch(const std::exception& e)
        {
            *error = e.what();
            return process.returncode;
        }

        if (output)
            *output = process.cout;
        if (error)
            *error = process.cerr;

        return process.returncode;
    }

    bool writeFile(
        const std::string& path, 
        const char* content,
        int contentSize)
    {
        std::ofstream fileStream(path);
        if (fileStream.fail())
            return false;
        
        fileStream.write(content, contentSize);
        fileStream.flush();
        fileStream.close();
        return true;
    }
}


namespace bgfxTools
{
    void SetToolDirectoryPath(
        const std::string& path,
        bool relative)
    {
        if (relative)
            g_toolPath = "./" + path;
        else
            g_toolPath = path;
    }

    bool CompileShaderFromString(
        const std::string& source,
        ShaderOptions& options,
        std::string* output,
        std::string* error,
        bool writeLibraries)
    {
        namespace fs = std::filesystem;

        static std::mutex mutex;
        static int count = 0;

        bool writeCommon = false;
        bool writeBgfxShader= false;
        bool writeShaderLib = false;

        if (options.inputFilePath.size() == 0)
        {
            if (error)
                *error = "No file path provided.";
            return false;
        }

        if (writeLibraries)
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (count == 0)
            {
                // Write common library files
                writeCommon = !fs::exists(FILE_COMMON_NAME);
                writeBgfxShader = !fs::exists(FILE_BGFXSHADER_NAME);
                writeShaderLib = !fs::exists(FILE_SHADERLIB_NAME);

                if (writeCommon)
                    writeFile(
                        g_toolPath + FILE_COMMON_NAME,
                        FILE_COMMON_CONTENT,
                        FILE_COMMON_SIZE);
                if (writeBgfxShader)
                    writeFile(
                        g_toolPath + FILE_BGFXSHADER_NAME,
                        FILE_BGFXSHADER_CONTENT,
                        FILE_BGFXSHADER_SIZE);
                if (writeShaderLib)
                    writeFile(
                        g_toolPath + FILE_SHADERLIB_NAME,
                        FILE_SHADERLIB_CONTENT,
                        FILE_SHADERLIB_SIZE);
            }
            count++;
        }

        // Write source to file

        bool result = CompileShader(options, output, error);

        if (writeLibraries)
        {
            std::lock_guard<std::mutex> lock(mutex);
            count--;
            if (count == 0)
            {
                // Remove common library files
                // Don't remove them if they already existed
                if (writeCommon)
                    fs::remove(g_toolPath + FILE_COMMON_NAME);
                if (writeBgfxShader)
                    fs::remove(g_toolPath + FILE_BGFXSHADER_NAME);
                if (writeShaderLib)
                    fs::remove(g_toolPath + FILE_SHADERLIB_NAME);
            }
        }

        return result;
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
