#include <windows.h>

bool running = true;

LRESULT CALLBACK WindowCallback(HWND WindowClass, UINT Message, WPARAM WParam, LPARAM LParam )
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
            DestroyWindow(WindowClass);
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
        
        case WM_SIZE: 
        {
            OutputDebugStringA("SIZE"); 
        }break;
        
        default:
        {
           Result = DefWindowProc(WindowClass, Message, WParam, LParam);    
        }break;
    }
    return Result;
}

int CALLBACK WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    WNDCLASSEXA WindowClass = {};
    WindowClass.cbSize = sizeof(WNDCLASSEXA);
    WindowClass.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
    WindowClass.lpfnWndProc = WindowCallback; //TODO 
    WindowClass.hInstance = hInst;
    WindowClass.hIcon = 0;
    WindowClass.lpszClassName = "Window Class";
    
    if(!RegisterClassExA(&WindowClass)){
        DebugBreak();
    }

    HWND WindowHandle = CreateWindowEx(0, WindowClass.lpszClassName, "Doom Pezzotto", WS_TILEDWINDOW | WS_VISIBLE,
                                CW_USEDEFAULT, CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT, 0, 0, hInst, 0);
    
    if(!WindowHandle){
        DebugBreak();
    }

    MSG message;
    while(running){
        while(PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
    DestroyWindow(WindowHandle);
    
    return 0;
} 
