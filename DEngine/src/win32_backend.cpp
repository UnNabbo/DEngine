#include "win32_backend.h"
#include <cmath>
#include <winuser.h>


struct {
	ivec2 Position;
	int8 Buttons;
} Mouse;
enum { MOUSE_LEFT = 0b1, MOUSE_MIDDLE = 0b10, MOUSE_RIGHT = 0b100, MOUSE_X1 = 0b1000, MOUSE_X2 = 0b10000 };

global char Keyboard[255] = {0};
global bool running = true;

global Bitmap* Backbuffer;

bool ShouldClose(){
    return !running;
}



void ClearBackground(int r, int g, int b){
    for(int y = 0; y < Backbuffer->Height; y++){
        for(int x = 0; x < Backbuffer->Width; x++){
            *(((uint32*)Backbuffer->Memory) + Backbuffer->Width * y + x ) = b | g << 8 | r << 16;
        }
    }
}

void Win32ResizeDIBSection(Bitmap * Buffer, int32 Width, int32 Height){
    
    if(!Buffer){
        return;
    }   

    if(Buffer->Memory){
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

    const uint32 SizeMod = 1;
    // Buffer->Height = Height;
    // Buffer->Width = Width;

    Buffer->Height = 128;
    Buffer->Width = 128;
    
    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;
    Buffer->Info.bmiHeader.biHeight = Buffer->Height;
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Size =  Buffer->Width * Buffer->Height * 4;
    Buffer->Memory = VirtualAlloc(0, Buffer->Size, MEM_COMMIT, PAGE_READWRITE); 
    memset(Buffer->Memory, 0x00000000, Buffer->Size); 
    
} 

void Win32DisplayBuffer(Bitmap* Buffer, HDC DeviceContext,int Width, int Height){
    if(!Buffer){
        return;
    }   

    StretchDIBits(DeviceContext,
                  0, 0, Width, Height,
                  0, 0, Buffer->Width, Buffer->Height,
                  Buffer->Memory,
                  &Buffer->Info,
                  DIB_RGB_COLORS,
                  SRCCOPY);

    memset(Buffer->Memory, 0, Buffer->Size); 
}

LRESULT CALLBACK WindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam )
{
    LRESULT Result = 0;
    switch(Message)
    {

        case WM_DESTROY: 
        {
            PostQuitMessage(0);
            running = false;
        }break;
        
        case WM_QUIT: 
        {
            OutputDebugStringA("QUIT"); 
            DestroyWindow(Window);
        }break;
        
        case WM_ACTIVATEAPP: 
        {
            OutputDebugStringA("FOCUSE"); 
        }break;
        
        case WM_CREATE: 
        {
            OutputDebugStringA("CREATED"); 
        }break;
        
        case WM_MOVE: 
        {
            OutputDebugStringA("MOVE"); 
        }break;
        
        case WM_SYSKEYUP:
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            uint32 VKCode = WParam;
            bool WasDown = ((LParam & (1 << 30)) != 0);
            bool IsDown = ((LParam & (1 << 31)) == 0);
            Keyboard[VKCode] = IsDown;

        }break;

		case WM_MOUSEMOVE: {
            ivec2 WSize = GetWindowSize(Window);
			Mouse.Position.x = (float)LOWORD(LParam) / WSize.x * Backbuffer->Width;
			Mouse.Position.y = Backbuffer->Height - ((float)HIWORD(LParam) / WSize.y * Backbuffer->Height);
		} break;

		case WM_LBUTTONDOWN:{
            Mouse.Buttons |=  MOUSE_LEFT;   
        }break;

        case WM_LBUTTONUP: {
        Mouse.Buttons &= ~MOUSE_LEFT;   
        }break;

		case WM_MBUTTONDOWN: {
            Mouse.Buttons |=  MOUSE_MIDDLE; 
        }break;

		case WM_MBUTTONUP: {
            Mouse.Buttons &= ~MOUSE_MIDDLE; 
        }break;

		case WM_RBUTTONDOWN: {
        Mouse.Buttons |=  MOUSE_RIGHT;  
        }break;

		case WM_RBUTTONUP: {
            Mouse.Buttons &= ~MOUSE_RIGHT;  
        }break;

        case WM_SIZE: 
        {
            OutputDebugStringA("SIZE"); 
            int Width = LOWORD(LParam);   
            int Height = HIWORD(LParam);
            Win32ResizeDIBSection(Backbuffer, Width, Height);

        }break;
        
        default:
        {
           Result = DefWindowProc(Window, Message, WParam, LParam);    
        }break;
    }
    return Result;
}

ivec2 GetWindowSize(HWND Window) {
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    int Width = ClientRect.right - ClientRect.left; 
    int Height = ClientRect.bottom - ClientRect.top;
    return {Width, Height};
}


void SetFrameBuffer(Window* Window, Bitmap* Buffer) {
    Backbuffer = Buffer;
    ivec2 WindowSize = GetWindowSize(*Window);
    Win32ResizeDIBSection(Backbuffer, WindowSize.Width, WindowSize.Height);   
}

Window MakeWindow(const char* Title, int Width, int Height){   
    WNDCLASSEXA WindowClass = {};
    WindowClass.cbSize = sizeof(WNDCLASSEXA);
    WindowClass.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
    WindowClass.lpfnWndProc = WindowCallback; //TODO 
    WindowClass.hInstance = GetModuleHandle(0);
    WindowClass.hIcon = 0;
    WindowClass.lpszClassName = "Window Class";
    
    if(!RegisterClassExA(&WindowClass)){
        DebugBreak();
    }
    
    Window Win = {};
    Win.Title = Title;
    Win.NativeWindow = CreateWindowEx(0, WindowClass.lpszClassName, "DEngine", WS_TILEDWINDOW | WS_VISIBLE,
                                CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, 0, 0, WindowClass.hInstance, 0);
    
    if(!Win.NativeWindow){
        running = false;
        DebugBreak();
    }
    return Win;
}

void PoolEvents(){
    MSG message;
    while(PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}

void DisplayBuffer(Window* WindowHandle){
        HDC DeviceContext = GetDC(*WindowHandle);
        ivec2 WindowSize = GetWindowSize(*WindowHandle);
        
        Win32DisplayBuffer(Backbuffer, DeviceContext, WindowSize.Width, WindowSize.Height);
        ReleaseDC(*WindowHandle, DeviceContext);
}

bool GetKeydown(int key){
    return Keyboard[key];
}


ivec2 GetMousePos(){
    return Mouse.Position;
}

void Quit(){
    running = false;
}
