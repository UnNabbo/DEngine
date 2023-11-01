#include "dmath.h"

#include "defines.h"

#include "win32_backend.h"
#include <debugapi.h>
#include <stdio.h>
#include <wingdi.h>

#define DistanceFN(x1, y1, x2, y2) sqrt( (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) ) 

#define CONCAT_IMPL( x, y ) x##y
#define MACRO_CONCAT( x, y ) CONCAT_IMPL( x, y )
#define Log(a,...) char CONCAT_IMPL(buff, __LINE__)[64]; sprintf(CONCAT_IMPL(buff, __LINE__), a, __VA_ARGS__); OutputDebugStringA(CONCAT_IMPL(buff, __LINE__));

void SetPixel(Bitmap * Buffer,int32 x, int32 y, uint8 r = 0xff, uint8 g = 0x00, uint8 b = 0xff){
    if((x < Buffer->Width && x > 0) && (y < Buffer->Height && y > 0)){
        *(((uint32*)Buffer->Memory) + Buffer->Width * y + x ) = b | g << 8 | r << 16;
    }
}


void DrawQuad(Bitmap* Buffer, int x1, int y1, int x2, int y2, vec3 color = {0xff, 0xff, 0xff}){
    int DeltaX = abs(x2 - x1);
    int SingX = (x1 < x2) ? 1 : -1;
    int DeltaY = 0;
    int SingY = (y1 < y2) ? 1 : -1;
    int Error =  DeltaX + DeltaY;

    int X = x1;
    int Y = (y1 < y2) ? y2 : y1 ;
    while(true){
        SetPixel(Buffer, X, Y, color.r, color.g, color.b);
        if(X == x2 && Y == y2) break;
        int Error2 = Error * 2;
        if(Error2 >= DeltaY){
            if(X == x2) break;
            Error += DeltaY;
            X += SingX;
        }

        if(Error2 <= DeltaX){
            if(Y == y2) break;
            Error += DeltaX;
            Y += SingY;
        }
    }
}

void DrawLine(Bitmap* Buffer, int x1, int y1, int x2, int y2, vec3 color = {0xff, 0xff, 0xff}){
    int DeltaX = abs(x2 - x1);
    int SingX = (x1 < x2) ? 1 : -1;
    int DeltaY = -abs(y2 - y1);
    int SingY = (y1 < y2) ? 1 : -1;
    int Error =  DeltaX + DeltaY;

    int X = x1;
    int Y = y1;
    while(true){
        SetPixel(Buffer, X, Y, color.r, color.g, color.b);
        if(X == x2 && Y == y2) break;
        int Error2 = Error * 2;
        if(Error2 >= DeltaY){
            if(X == x2) break;
            Error += DeltaY;
            X += SingX;
        }

        if(Error2 <= DeltaX){
            if(Y == y2) break;
            Error += DeltaX;
            Y += SingY;
        }
    }
}


void DrawLine(Bitmap* Buffer, vec2 Point1, vec2 Point2, vec3 color = {0xff, 0xff, 0xff}){
    DrawLine(Buffer, Point1.x, Point1.y, Point2.x, Point2.y, color);
}


void DrawQuad(Bitmap* Buffer, vec2 Point1, vec2 Point2, vec3 color = {0xff, 0xff, 0xff}){
    DrawQuad(Buffer, Point1.x, Point1.y, Point2.x, Point2.y, color);
}

#define DEBUG_LINE(x, y, l) DrawLine(Buffer, x, y, x, y + l, {0xff, 0, 0}); DrawLine(Buffer, x, y, x + l, y , {0, 0xff, 0}); DrawLine(Buffer, x, y, x, y , {0, 0, 0xff})

struct PlayerData{
    vec3 Pos = {0, 0, -64};
    float Yaw = 0;
    float Pitch = 0;
};

PlayerData Player = {};

#define RotatePointX(Pivot, Point, Angle) (Point.x - Pivot.x) * (float)cos(Angle) - (Point.z - Pivot.z) * (float)sin(Angle);
#define RotatePointY(Pivot, Point, Angle) (Point.x - Pivot.x) * (float)sin(Angle) + (Point.z - Pivot.z) * (float)cos(Angle);

vec2 GProjectPoint(vec3 Point, vec3 Pivot, float Angle){
    
    vec3 RPoint = {};
    RPoint.x =  RotatePointX(Pivot, Point, Angle);
    RPoint.z =  RotatePointY(Pivot, Point, Angle);
    RPoint.y =  0 - Point.y - Pivot.y;
    vec2 TPoint = {};
    TPoint.x = RPoint.x * 200.0f / RPoint.z + 64;
    TPoint.y = RPoint.y * 200.0f / RPoint.z + 64;
    return TPoint;
}

void GameUpdateAndRender(Bitmap* Buffer){
    ivec2 pos = GetMousePos();
    SetPixel(Buffer, pos.x, pos.y);
    Log("%i", pos.x);

    float DeltaMove = 0.1;
    if(GetKeydown('W')){
        Player.Pos.z += DeltaMove * cos(Player.Yaw);
        Player.Pos.x += DeltaMove * sin(Player.Yaw);
    }

    if(GetKeydown('S')){
        Player.Pos.z -= DeltaMove * cos(Player.Yaw);
        Player.Pos.x -= DeltaMove * sin(Player.Yaw);
    }


    if(GetKeydown('D')){
        Player.Pos.z -= DeltaMove * sin(Player.Yaw);
        Player.Pos.x += DeltaMove * cos(Player.Yaw);
    }

    
    if(GetKeydown('A')){
        Player.Pos.z += DeltaMove * sin(Player.Yaw);
        Player.Pos.x -= DeltaMove * cos(Player.Yaw);
    }

    if(GetKeydown('Q')){
        Player.Yaw -= 0.001;
    }

    if(GetKeydown('E')){

        Player.Yaw += 0.001;
    }

    if(GetKeydown('R')){
        Player = {};
    }

    vec3 Center = {0,0,0};
    vec2 TCenter = GProjectPoint(Center, Player.Pos, Player.Yaw);
    SetPixel(Buffer, TCenter.x, TCenter.y, 0xff, 0, 0xff);
    
    vec3 Point1 = {-32, 16, 64};
    vec2 TPoint1 = GProjectPoint(Point1, Player.Pos, Player.Yaw);
    
    vec3 Point2 = {32, 16, 64};
    vec2 TPoint2 = GProjectPoint(Point2, Player.Pos, Player.Yaw);

    
    vec3 Point3 = {-32, -16, 64};
    vec2 TPoint3 = GProjectPoint(Point3, Player.Pos, Player.Yaw);

    vec3 Point4 = {32, -16, 64};
    vec2 TPoint4 = GProjectPoint(Point4, Player.Pos, Player.Yaw);
    

    DrawQuad(Buffer, TPoint1, TPoint4);
    SetPixel(Buffer, TPoint3.x, TPoint3.y, 0xff,0,0);
    SetPixel(Buffer, TPoint4.x, TPoint4.y, 0xff,0,0);
    SetPixel(Buffer, TPoint1.x, TPoint1.y, 0xff,0,0);
    SetPixel(Buffer, TPoint2.x, TPoint2.y, 0xff,0,0);
    // DrawLine(Buffer, TPoint3, TPoint2);



#if 1
    DEBUG_LINE(16, 16, 8);
#endif
    SetPixel(Buffer, Player.Pos.x, Player.Pos.y);
}


int CALLBACK WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int32 cmdshow)
{
    
    Bitmap Backbuffer = {};
    Window Window = MakeWindow("DEngine", 1024, 1024);
    
    SetFrameBuffer(&Window,&Backbuffer);
    

    while(!ShouldClose()){
        PoolEvents();
        ClearBackground(31,32,34);
           
        GameUpdateAndRender(&Backbuffer);
        DisplayBuffer(&Window);
    }

    return 0;
} 
