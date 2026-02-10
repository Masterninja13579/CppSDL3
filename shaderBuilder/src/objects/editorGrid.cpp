#include "editorGrid.h"

#include "bgfxTools/bgfxTools.h"

#include <bx/math.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

namespace
{
    struct Line
    {
        uint16_t a;
        uint16_t b;
    };

    const std::string VARYING_SOURCE =
        "vec3 v_position : TEXCOORD0;\n"
        "\n"
        "vec3 a_position : POSITION;";
    const std::string VERTEX_SHADER_SOURCE =
        "$input a_position\n"
        "$output v_position\n"
        "\n"
        "#include \"./common.sh\"\n"
        "\n"
        "uniform vec4 u_scaleDist00;\n"
        "\n"
        "#define u_scale u_scaleDist00.x\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = u_scale * mul(u_modelViewProj, vec4(a_position, 1.0));\n"
        "    v_position = a_position;\n"
        "}";
    const std::string FRAGMENT_SHADER_SOURCE = 
        "$input v_position\n"
        "\n"
        "#include \"./common.sh\"\n"
        "\n"
        "uniform vec4 u_nearColor;\n"
        "uniform vec4 u_farColor;\n"
        "uniform vec4 u_scaleDist00;\n"
        "\n"
        "#define u_dist u_scaleDist00.y"
        "\n"
        "void main()\n"
        "{\n"
        "    float dist = sqrt(pow(v_position.x, 2) + pow(v_position.z, 2));\n"
        "    float p = clamp(dist / u_dist, 0, 1);\n"
        "    float r = mix(u_nearColor.r, u_farColor.r, p);\n"
        "    float g = mix(u_nearColor.g, u_farColor.g, p);\n"
        "    float b = mix(u_nearColor.b, u_farColor.b, p);\n"
        "    float a = mix(u_nearColor.a, u_farColor.a, p);\n"
        "    gl_FragColor = vec4(r, g, b, a);\n"
        "}";
}

bool EditorGrid::initialized = false;
uint64_t EditorGrid::sRenderState;
bgfx::VertexLayout EditorGrid::sVertexLayout;
bgfx::ProgramHandle EditorGrid::sProgramHandle;
bgfx::UniformHandle EditorGrid::sUniformNearColor;
bgfx::UniformHandle EditorGrid::sUniformFarColor;
bgfx::UniformHandle EditorGrid::sUniformScaleDist00;

void EditorGrid::construct(int width, int height)
{
    if (width <= 0 || height <= 0)
        return;

    float widthf = width;
    float heightf = height;

    float endX = widthf / 2.0f;
    float endY = heightf / 2.0f;
    float startX = -endX;
    float startY = -endY;

    scaleDist00[1] = bx::sqrt(endX * endX + endY * endY);

    int vertexCount = 4 + 2*(width - 1) + 2*(height - 1);
    int lineCount = width + height + 2;
    
    // Create vertex buffer
    vertexMemory = new uint8_t[vertexCount * sizeof(Vertex)];
    Vertex* vertexBuffer = (Vertex*)vertexMemory;
    {
        // Corners
        vertexBuffer[0] = { startX, 0.0f, startY };
        vertexBuffer[1] = { endX, 0.0f, startY };
        vertexBuffer[2] = { endX, 0.0f, endY };
        vertexBuffer[3] = { startX, 0.0f, endY };
        // Filler vertical lines
        int index = 3;
        for (int i = 0; i < width - 1; ++i)
        {
            float percent = (i + 1) / widthf;
            float x = startX + percent*widthf;
            vertexBuffer[++index] = { x, 0.0f, startY };
            vertexBuffer[++index] = { x, 0.0f, endY };
        }
        // Filler horizontal lines
        for (int i = 0; i < height - 1; ++i)
        {
            float percent = (i + 1) / heightf;
            float y = startY + percent*heightf;
            vertexBuffer[++index] = { startX, 0.0f, y };
            vertexBuffer[++index] = { endX, 0.0f, y };
        }
    }

    // Create index buffer for lines
    indexMemory = new uint8_t[lineCount * sizeof(Line)];
    Line* indexBuffer = (Line*)indexMemory;
    {
        // Edge lines
        indexBuffer[0] = { 0, 1 };
        indexBuffer[1] = { 1, 2 };
        indexBuffer[2] = { 2, 3 };
        indexBuffer[3] = { 3, 0 };
        // Filler vertical lines
        int index = 3;
        for (int i = 0; i < width - 1; ++i)
        {
            uint16_t a = 4 + 2*i;
            uint16_t b = a + 1;
            indexBuffer[++index] = { a, b };
        }
        // Filler horizontal lines
        int hs = 4 + 2*(width - 1);
        for (int i = 0; i < height - 1; ++i)
        {
            uint16_t a = hs + 2*i;
            uint16_t b = a + 1;
            indexBuffer[++index] = { a, b };
        }
    }

    vertexBufferHandle = bgfx::createVertexBuffer(
        bgfx::makeRef(
            vertexMemory, 
            vertexCount * sizeof(Vertex)
        ),
        sVertexLayout
    );
    indexBufferHandle = bgfx::createIndexBuffer(
        bgfx::makeRef(
            indexMemory,
            lineCount * sizeof(Line)
        )
    );
}

EditorGrid::EditorGrid(int width, int height, float scale)
{
    scaleDist00[0] = scale;
    construct(width, height);
}
EditorGrid::~EditorGrid()
{
    bgfx::destroy(vertexBufferHandle);
    bgfx::destroy(indexBufferHandle);
}

void EditorGrid::setNearColor(float r, float g, float b, float a)
{
    nearColor[0] = r;
    nearColor[1] = g;
    nearColor[2] = b;
    nearColor[3] = a;
}
void EditorGrid::setNearColor(int r, int g, int b, int a)
{
    setNearColor(
        r / 255.0f,
        g / 255.0f,
        b / 255.0f,
        a / 255.0f
    );
}
void EditorGrid::setFarColor(float r, float g, float b, float a)
{
    farColor[0] = r;
    farColor[1] = g;
    farColor[2] = b;
    farColor[3] = a;
}
void EditorGrid::setFarColor(int r, int g, int b, int a)
{
    setFarColor(
        r / 255.0f,
        g / 255.0f,
        b / 255.0f,
        a / 255.0f
    );
}

void EditorGrid::render()
{
    float transformMatrix[16];
    bx::mtxIdentity(transformMatrix);
    bgfx::setTransform(transformMatrix);

    bgfx::setVertexBuffer(0, vertexBufferHandle);
    bgfx::setIndexBuffer(indexBufferHandle);

    bgfx::setState(sRenderState);

    bgfx::setUniform(sUniformNearColor, &nearColor);
    bgfx::setUniform(sUniformFarColor, &farColor);
    bgfx::setUniform(sUniformScaleDist00, &scaleDist00);

    bgfx::submit(0, sProgramHandle);
}

bool EditorGrid::Initialize()
{
    if (initialized)
        return true;

    sRenderState = BGFX_STATE_WRITE_R
                 | BGFX_STATE_WRITE_G
                 | BGFX_STATE_WRITE_B
                 | BGFX_STATE_WRITE_A
                 | BGFX_STATE_WRITE_Z
                 | BGFX_STATE_DEPTH_TEST_LESS
                 | BGFX_STATE_CULL_CW
                 | BGFX_STATE_MSAA
                 | BGFX_STATE_PT_LINES
                 | BGFX_STATE_BLEND_SRC_ALPHA;

    sVertexLayout
        .begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .end();

    {
        std::ofstream file("varying.def.sc");
        file.write(VARYING_SOURCE.c_str(), VARYING_SOURCE.size());
    }

    std::string toolOutput = "";
    std::string toolError = "";
    bgfx::Memory* vShaderData;
    bgfx::Memory* fShaderData;
    bool compileResult = false;
    bgfxTools::StringShaderOptions options;

    bgfxTools::SetToolDirectoryPath("../");

    compileResult = bgfxTools::CompileShaderFromString(
        VERTEX_SHADER_SOURCE,
        options,
        &vShaderData,
        &toolOutput,
        &toolError);
    if (!compileResult)
    {
        std::cout << "Vertex shader compilation failed:\n";
        if (toolOutput.size() > 0)
        {
            std::cout << "-------- Output --------\n";
            std::cout << toolOutput << "\n";
        }
        if (toolError.size() > 0)
        {
            std::cout << "-------- Error --------\n";
            std::cout << toolError << "\n";
        }
        return false;
    }
    options.type = bgfxTools::ShaderOptions::Type::Fragment;
    compileResult = bgfxTools::CompileShaderFromString(
        FRAGMENT_SHADER_SOURCE,
        options,
        &fShaderData,
        &toolOutput,
        &toolError);
    if (!compileResult)
    {
        std::cout << "Fragment shader compilation failed:\n";
        if (toolOutput.size() > 0)
        {
            std::cout << "-------- Output --------\n";
            std::cout << toolOutput << "\n";
        }
        if (toolError.size() > 0)
        {
            std::cout << "-------- Error --------\n";
            std::cout << toolError << "\n";
        }
        return false;
    }

    fs::remove("varying.def.sc");

    bgfx::ShaderHandle vertexHandle = bgfx::createShader(vShaderData);
    bgfx::ShaderHandle fragmentHandle = bgfx::createShader(fShaderData);
    sProgramHandle = bgfx::createProgram(vertexHandle, fragmentHandle, true);

    sUniformNearColor = bgfx::createUniform("u_nearColor", bgfx::UniformType::Vec4);
    sUniformFarColor = bgfx::createUniform("u_farColor", bgfx::UniformType::Vec4);
    sUniformScaleDist00 = bgfx::createUniform("u_scaleDist00", bgfx::UniformType::Vec4);

    initialized = true;
    return true;
}

