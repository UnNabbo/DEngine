
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


enum { MOUSE_LEFT = 0b1, MOUSE_MIDDLE = 0b10, MOUSE_RIGHT = 0b100, MOUSE_X1 = 0b1000, MOUSE_X2 = 0b10000 };

void SetFramebuffer(Window* Window, Bitmap* bitmap);
Bitmap * GetFramebuffer();
Window MakeWindow(const char * Title, int Width = CW_USEDEFAULT, int Height = CW_USEDEFAULT);
void DestroyWindow(Window*);
ivec2 GetWindowSize(HWND Window);
bool GetKeydown(int key);
ivec2 GetMousePos();
bool ShouldClose();
void CloseWindow();
void PoolEvents(); 
void DisplayBuffer(Window* WindowHandle);
