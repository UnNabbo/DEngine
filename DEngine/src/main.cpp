#include "dmath.h"

#include "defines.h"

#include "logger.h"

#include "win32_backend.h"

#include "rendering.h"

void GameUpdateAndRender(Bitmap* Buffer){
    ivec2 pos = GetMousePos();
    #if 1
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
#endif

    DrawBezier({63,95}, {95, 95}, {95, 63});

    DrawBezier({95,63}, {96, 32}, {63, 32});

    DrawBezier({63,32}, {32, 32}, {32, 63});

    DrawBezier({32,63}, {32, 95}, {63, 95});

}


int CALLBACK WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int32 cmdshow)
{
    
    Bitmap Backbuffer = {};
    Window Window = MakeWindow("DEngine", 1024, 1024);
    
    SetFramebuffer(&Window, &Backbuffer);
        


    while(!ShouldClose()){
        PoolEvents();

        BeginRendering();
        ClearBackground(0x21,0x22,0x24);
           

        GameUpdateAndRender(&Backbuffer);
        EndRendering();
        DisplayBuffer(&Window);


    }

    return 0;
} 
