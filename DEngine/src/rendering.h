#pragma once

#include "dmath.h"

struct VertexArray {
    vec2* Vertices;
    size_t Size;
    uint32 Count;
};

struct Bitmap;

void ClearBackground(int r = 0, int g = 0, int b = 0);

void UnorderedDraw(VertexArray Va, vec3 color, uint32 Begin = 0);

void BeginRendering();
void EndRendering();


void DrawBezier(ivec2 p0, ivec2 p1, ivec2 p2, vec3 Color = {0xff, 0, 0});
void SetPixel(int32 x, int32 y, uint8 r = 0xff, uint8 g = 0x00, uint8 b = 0xff);
