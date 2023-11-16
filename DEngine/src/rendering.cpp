#include "rendering.h"

#include "defines.h"

#include "win32_backend.h"

#include "dmath.h"

#include "logger.h"

global Bitmap * Framebuffer = 0;

//INTERNAL FUNCTIONS
INLINE internal void SetPixel(int32 x, int32 y, uint8 r = 0xff, uint8 g = 0x00, uint8 b = 0xff){
    if((x < Framebuffer->Width && x >= 0) && (y < Framebuffer->Height && y >= 0)){
        *(((uint32*)Framebuffer->Memory) + Framebuffer->Width * y + x ) = b | g << 8 | r << 16;
    }
}


INLINE internal vec3 GetPixel(int32 x, int32 y){
    vec3 Color = {};
    if((x < Framebuffer->Width && x >= 0) && (y < Framebuffer->Height && y >= 0)){

        uint32 RawColor = *(((uint32*)Framebuffer->Memory) + Framebuffer->Width * y + x );
        Color.b = RawColor & 0x00000FF;
        Color.g = RawColor & 0x0000FF00;
        Color.r = RawColor & 0x00FF0000;
    }
    return Color;
}


internal void DrawLine(int x1, int y1, int x2, int y2, vec3 color = {0xff, 0xff, 0xff}){
    int DeltaX = abs(x2 - x1);
    int SingX = (x1 < x2) ? 1 : -1;
    int DeltaY = -abs(y2 - y1);
    int SingY = (y1 < y2) ? 1 : -1;
    int Error =  DeltaX + DeltaY;

    int X = x1;
    int Y = y1;
    while(true){
        SetPixel(X, Y, color.r, color.g, color.b);
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


void DrawLine(vec2 Point1, vec2 Point2, vec3 color = {0xff, 0xff, 0xff}){
    DrawLine(Point1.x, Point1.y, Point2.x, Point2.y, color);
}
//End

#define ToScreenPos(v) ivec2{(int)(v.x * (Framebuffer->Width / 2.f) + (Framebuffer->Width / 2.f)), (int)(v.y * (Framebuffer->Height/ 2.f) + (Framebuffer->Height/ 2.f))}

internal int EdgeFunction(ivec2 a, ivec2 b, ivec2 p){
    ivec2 ab = b-a;
    ivec2 ap = p-a;
    return CrossProduct2D(ab, ap);
}


internal bool EdgeBias(ivec2 a, ivec2 b){
    ivec2 ab = b-a;
    return (ab.y == 0 && ab.x > 0 ) || (ab.y < 0) ;
}

void UnorderedDraw(VertexArray Va, vec3 color, uint32 Begin){
    for(uint32 i = Begin; i < Va.Count - 1; i += 3){
        ivec2 Pos1 = ToScreenPos(Va.Vertices[i]);
        ivec2 Pos2 = ToScreenPos(Va.Vertices[i + 1]);
        ivec2 Pos3 = ToScreenPos(Va.Vertices[i + 2]);

        vec3 Color1 = {0xff, 0, 0};
        vec3 Color2 = {0, 0xff, 0};
        vec3 Color3 = {0, 0 , 0xff};

        int min_x = max(vmin(Pos1.x, Pos2.x, Pos3.x), 0 );
        int max_x = min(vmax(Pos1.x, Pos2.x, Pos3.x), Framebuffer->Width);

        int min_y = max(vmin(Pos1.y, Pos2.y, Pos3.y), 0);
        int max_y = min(vmax(Pos1.y, Pos2.y, Pos3.y), Framebuffer->Height);
        
        float Area = EdgeFunction(Pos1, Pos2, Pos3);

        int bias1 = EdgeBias(Pos2, Pos3) ? 0 : -1;
        int bias2 = EdgeBias(Pos3, Pos1) ? 0 : -1;
        int bias3 = EdgeBias(Pos1, Pos2) ? 0 : -1;
        
        int dw1_y = Pos3.x - Pos2.x;
        int dw2_y = Pos1.x - Pos3.x;
        int dw3_y = Pos2.x - Pos1.x;

        int dw1_x = Pos2.y - Pos3.y;
        int dw2_x = Pos3.y - Pos1.y;
        int dw3_x = Pos1.y - Pos2.y;

        ivec2 StartP = {min_x, min_y};
        int w1b = EdgeFunction(Pos2, Pos3, StartP) + bias1;
        int w2b = EdgeFunction(Pos3, Pos1, StartP) + bias2;
        int w3b = EdgeFunction(Pos1, Pos2, StartP) + bias3;

        for(int y = min_y; y <= max_y; y++){
            int w1 = w1b;
            int w2 = w2b;
            int w3 = w3b;
            for(int x = min_x; x <= max_x; x++){
                ivec2 Pixel= {x, y};


                if(w1 >= 0 && w2 >= 0 && w3 >= 0){
                    float alpha = w1 / Area;   
                    float beta  = w2 / Area;   
                    float gamma = w3 / Area;   
                    
                    int R = alpha * Color1.r + beta * Color2.r + gamma * Color3.r;
                    int G = alpha * Color1.g + beta * Color2.g + gamma * Color3.g;
                    int B = alpha * Color1.b + beta * Color2.b + gamma * Color3.b;
                    SetPixel(x,y, R, B, G);
                }
                w1 += dw1_x;
                w2 += dw2_x;
                w3 += dw3_x;
            }
            w1b += dw1_y;
            w2b += dw2_y;
            w3b += dw3_y;
        }
    }
} 

void BeginRendering(){
  Framebuffer = GetFramebuffer();
    if(!Framebuffer){
        //ASSERT
    }
}


void EndRendering(){
    Framebuffer = 0;
}

void ClearBackground(int r, int g, int b){
    for(int y = 0; y < Framebuffer->Height; y++){
        for(int x = 0; x < Framebuffer->Width; x++){
            *(((uint32*)Framebuffer->Memory) + Framebuffer->Width * y + x ) = b | g << 8 | r << 16;
        }
    }
}
