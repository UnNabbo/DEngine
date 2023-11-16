#include "dmath.h"

#include "defines.h"

#include "logger.h"

#include "win32_backend.h"

#include "rendering.h"

void GameUpdateAndRender(Bitmap* Buffer){
    ivec2 pos = GetMousePos();

    {
    vec2 vertices[] = {
        {-0.5f, -0.5f},
        { 0.5f, -0.5f},
        { 0.0f,  0.5f},
        

    };

    VertexArray va;
    va.Vertices = vertices;
    va.Count = 3;
    va.Size = sizeof(vertices);
    UnorderedDraw(va, {0xff,0,0});
    }

    {
    vec2 vertices[] = {
        

        { 0.0f,  0.5f},
        { -0.5f,  0.8f},
        {-0.5f, -0.5f},
    };

    VertexArray va;
    va.Vertices = vertices;
    va.Count = 3;
    va.Size = sizeof(vertices);
    UnorderedDraw(va, {0,0xff,0});
    }


    {
    vec2 vertices[] = {
        {0.5f, -0.5f},
        { .5f,  0.8f},
        { 0.0f,  0.5f},
    };

    VertexArray va;
    va.Vertices = vertices;
    va.Count = 3;
    va.Size = sizeof(vertices);
    UnorderedDraw(va, {0,0,0xff});
    }


    {
    vec2 vertices[] = {
        { 0.0f,  0.5f},
        { .5f,  0.8f},
        { -.5f,  0.8f},
    };

    VertexArray va;
    va.Vertices = vertices;
    va.Count = 3;
    va.Size = sizeof(vertices);
    UnorderedDraw(va, {0,0xff,0xff});
    }


    {
    vec2 vertices[] = {
        { .5f,  0.8f},
        { 0.0f,  1.0f},
        { -.5f,  0.8f},
    };

    VertexArray va;
    va.Vertices = vertices;
    va.Count = 3;
    va.Size = sizeof(vertices);
    UnorderedDraw(va, {0xff,0xff,0xff});
    }
}


int CALLBACK WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int32 cmdshow)
{
    
    Bitmap Backbuffer = {};
    Window Window = MakeWindow("DEngine", 1024, 1024);
    
    SetFramebuffer(&Window, &Backbuffer);
    
    while(!ShouldClose()){
        PoolEvents();
        BeginRendering();
        ClearBackground(31,32,34);
           
        GameUpdateAndRender(&Backbuffer);
        EndRendering();

        DisplayBuffer(&Window);
    }

    return 0;
} 
