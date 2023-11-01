
#include <windows.h>

#include "defines.h"

#include "dmath.h"

struct Bitmap{
    BITMAPINFO Info;
    int32 Height, Width;
    void * Memory;
    uint32 Size;
};

struct Window{
    const char * Title;
    int Width, Height;
    HWND NativeWindow;
    
    operator HWND(){ return NativeWindow; }
};


void SetFrameBuffer(Window* Window, Bitmap* bitmap);
Window MakeWindow(const char * Title, int Width = CW_USEDEFAULT, int Height = CW_USEDEFAULT);
ivec2 GetWindowSize(HWND Window);
bool GetKeydown(int key);
ivec2 GetMousePos();
bool ShouldClose();
void CloseWindow();
void PoolEvents(); 
void DisplayBuffer(Window* WindowHandle);
void ClearBackground(int r = 0, int g = 0, int b = 0);
