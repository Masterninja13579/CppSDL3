#pragma once

#include "core.h"

class EditorGrid
{
    struct Vertex
    {
        float x;
        float y;
        float z;
    };
    
    float nearColor[4] = { 0.53f, 0.74f, 0.82f, 0.7f };
    float farColor[4] = { 0.53f, 0.74f, 0.82f, 0.4f };
    float scaleDist00[4] { 1.0f, 0.0f, 0.0f, 0.0f };
    uint8_t* vertexMemory = nullptr;
    uint8_t* indexMemory = nullptr;
    bgfx::VertexBufferHandle vertexBufferHandle;
    bgfx::IndexBufferHandle indexBufferHandle;

    static bool initialized;
    static uint64_t sRenderState;
    static bgfx::VertexLayout sVertexLayout;
    static bgfx::ProgramHandle sProgramHandle;
    static bgfx::UniformHandle sUniformNearColor;
    static bgfx::UniformHandle sUniformFarColor;
    static bgfx::UniformHandle sUniformScaleDist00;

    void construct(int width, int height);

public:
    EditorGrid() = delete;
    EditorGrid(const EditorGrid&) = delete;
    EditorGrid(
        int width,
        int height,
        float scale = 1.0f
    );
    ~EditorGrid();

    void setNearColor(float r, float g, float b, float a);
    void setNearColor(int r, int g, int b, int a);
    void setFarColor(float r, float g, float b, float a);
    void setFarColor(int r, int g, int b, int a);
    void setScale(float scale);

    void render();

    static bool Initialize();
};
