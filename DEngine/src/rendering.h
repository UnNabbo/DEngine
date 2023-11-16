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
