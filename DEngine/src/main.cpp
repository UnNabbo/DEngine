#include "dmath.h"

#include "defines.h"

#include "logger.h"


#include "memalloc.h"
#include "system.h"
#include "win32_backend.h"

#include "memarena.h"

#include "rendering.h"

#include "arrays.h"

void GameUpdateAndRender(Bitmap* Buffer, MemArena * FrameMemory){
    ClearBackground(0x21,0x22,0x24);
    
    void * m1 = MemAlloc(32);
    void * m2 = MemAlloc(32);
    void * m3 = MemAlloc(64);
    MemFree(m1);
    // __debugbreak();
    MemFree(m2);
    MemFree(m3);

    Array<int, 4> a = {};

    #if 0
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
}


int CALLBACK WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int32 cmdshow)
{
    
    Bitmap Backbuffer = {};
    Window Window = MakeWindow("DEngine", 1024, 1024);
    
    SetFramebuffer(&Window, &Backbuffer);
    RetriveSystemInfo();
    
    uint32 MemSize = GB(1);
    MemArena Arena = AllocateMemArena(MemSize);
    MemArena GameMem = AllocateSubMemArena(&Arena, MemSize / 2);
    MemArena RenderMem = AllocateSubMemArena(&Arena, MemSize / 2);
    while(!ShouldClose()){
        PoolEvents();

        BeginRendering();
           
        GameUpdateAndRender(&Backbuffer, &GameMem);

        FreeMemArena(&Arena);

        EndRendering();
        DisplayBuffer(&Window);


    }

    DumpMemDebug();
    return 0;
}
