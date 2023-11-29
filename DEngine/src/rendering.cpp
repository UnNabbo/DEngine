#include "rendering.h"

#include "defines.h"

#include "memarena.h"
#include "win32_backend.h"

#include "dmath.h"

#include "logger.h"

global Bitmap * Framebuffer = 0;
global MemArena * RenderingMemory = 0; 

void SetPixel(int32 x, int32 y, uint8 r , uint8 g, uint8 b){
    //if((x < Framebuffer->Width && x >= 0) && (y < Framebuffer->Height && y >= 0)){
    *(((uint32*)Framebuffer->Memory) + Framebuffer->Width * y + x) = b | g << 8 | r << 16;
    //}
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
            Error += DeltaY;
            X += SingX;
        }

        if(Error2 <= DeltaX){
            Error += DeltaX;
            Y += SingY;
        }
    }

}


void DrawLine(ivec2 Point1, ivec2 Point2, vec3 color = {0xff, 0xff, 0xff}){
    DrawLine(Point1.x, Point1.y, Point2.x, Point2.y, color);
}

void PlotBezier(int x0, int y0, int x1, int y1, int x2, int y2, vec3 Color) 
{ 
    int SX = x2-x1;
    int SY = y2-y1;
    long RelativeX = x0-x1;
    long RelativeY = y0-y1;
    double Curve = RelativeX*SY-RelativeY*SX; /* curvature */

    double DeltaX, DeltaY, Error;
    long Delta2XY; /* relative values for checks */
    
    if (SX*(long)SX+SY*(long)SY > RelativeX*RelativeX+RelativeY*RelativeY) { /* begin with longer part */
      x2 = x0; x0 = SX+x1; y2 = y0; y0 = SY+y1; Curve = -Curve; /* swap P0 P2 */
    }

    if (Curve != 0) { /* no straight line */
        RelativeX += SX; RelativeX *= SX = x0 < x2 ? 1 : -1; /* x step direction */
        RelativeY += SY; RelativeY *= SY = y0 < y2 ? 1 : -1; /* y step direction */
    
        Delta2XY = 2 * RelativeX * RelativeY; 
    
        RelativeX *= RelativeX;
        RelativeY *= RelativeY; /* differences 2nd degree */
    
        if (Curve * SX * SY < 0) { /* negated curvature? */
            RelativeX = -RelativeX; RelativeY = -RelativeY; Delta2XY = -Delta2XY; Curve = -Curve;
        }

        DeltaX = 4.0 * SY * Curve*(x1-x0)+RelativeX-Delta2XY; /* differences 1st degree */
        DeltaY = 4.0 * SX * Curve*(y0-y1)+RelativeY-Delta2XY;

        RelativeX += RelativeX;
        RelativeY += RelativeY; 

        Error = DeltaX + DeltaY + Delta2XY; /* error 1st step */
    
        do {
            SetPixel(x0,y0, Color.r, Color.g, Color.b); /* plot curve */
            if (x0 == x2 && y0 == y2) return; /* last pixel -> curve finished */
    
            y1 = 2*Error < DeltaX; /* save value for test of y step */
    
            if (2*Error > DeltaY) {
                x0 += SX;
                DeltaX -= Delta2XY;
                DeltaY += RelativeY;
                Error += DeltaY;
            } /* x step */
    
            if ( y1 ) {
                y0 += SY; 
                DeltaY -= Delta2XY;
                DeltaX += RelativeX; 
                Error += DeltaX;
            } /* y step */
        } while (DeltaY < 0 && DeltaX > 0); /* gradient negates -> algorithm fails */
    }
    DrawLine(x0,y0, x2,y2, Color); /* plot remaining part to end */
}

void DrawBezier(int x0, int y0, int x1, int y1, int x2, int y2, vec3 Color) 
{
    int RelativeX = x0-x1;
    int RelativeY = y0-y1;
    double T = x0 - 2*x1 + x2;

    double Intersection;

    if ((long)RelativeX*(x2-x1) > 0) { /* horizontal cut at P4? */
        if ((long)RelativeY*(y2-y1) > 0){ /* vertical cut at P6 too? */
            if (fabs((y0-2*y1+y2)/T*RelativeX) > abs(RelativeY)) { /* which first? */
                x0 = x2;
                x2 = RelativeX+x1; 
                y0 = y2;
                y2 = RelativeY+y1; /* swap points */
            } /* now horizontal cut at P4 comes first */
        }
        T = (x0 - x1) / T;
        Intersection = (1 - T) * ((1 - T) * y0 + 2.0 * T * y1) + T * T * y2; /* By(t=P4) */

        T = (x0 * x2 - x1 * x1) * T / (x0 - x1); /* gradient dP4/dx=0 */
        RelativeX = floor(T + 0.5);
        RelativeY = floor(Intersection + 0.5);
        Intersection = (y1 - y0) * (T - x0) / (x1 - x0) + y0; /* intersect P3 | P0 P1 */

        PlotBezier(x0, y0, RelativeX, floor(Intersection+0.5), RelativeX, RelativeY, Color);

        Intersection = (y1 - y2) * (T - x2) / (x1 - x2) + y2; /* intersect P4 | P1 P2 */

        x1 = RelativeX; 
        x0 = x1;

        y1 = floor(Intersection+0.5); /* P0 = P4, P1 = P8 */
        y0 = RelativeY; 
    }

    if ( (long)(y0-y1) * (y2 - y1) > 0) { /* vertical cut at P6? */
        T = y0 - 2 * y1 + y2;

        T = (y0 - y1) / T;
        Intersection = (1 - T) * ((1 - T) * x0 + 2.0 * T * x1) + T * T * x2; /* Bx(t=P6) */

        T = (y0 * y2 - y1 * y1) * T / (y0 - y1); /* gradient dP6/dy=0 */
        RelativeX = floor(Intersection + 0.5);
        RelativeY = floor(T + 0.5);
        Intersection = (x1 - x0)*( T - y0) / (y1 - y0) + x0; /* intersect P6 | P0 P1 */

        PlotBezier(x0, y0, floor(Intersection + 0.5), RelativeY, RelativeX, RelativeY, Color);

        Intersection = (x1 - x2) * (T - y2) / (y1 - y2) + x2; /* intersect P7 | P1 P2 */
   
        x1 = floor(Intersection+0.5); 
        x0 = RelativeX;
        
        y1 = RelativeY; /* P0 = P6, P1 = P7 */
        y0 = y1;
    }

    PlotBezier(x0,y0, x1,y1, x2,y2, Color); /* remaining part */ 
}

void DrawBezier(ivec2 p0, ivec2 p1, ivec2 p2, color3 Color){
    DrawBezier(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, Color);    
}

//End
//

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
        
        float Area = 1.0f / EdgeFunction(Pos1, Pos2, Pos3);

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
                    float alpha = w1 * Area;   
                    float beta  = w2 * Area;   
                    float gamma = w3 * Area;   
                    
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


void ClearBackground(color3 color){
    ClearBackground(color.r, color.g, color.b);
}

void ClearBackground(int r, int g, int b){
    for(int y = 0; y < Framebuffer->Height; y++){
        for(int x = 0; x < Framebuffer->Width; x++){
            *(((uint32*)Framebuffer->Memory) + Framebuffer->Width * y + x ) = b | g << 8 | r << 16;
        }
    }
}
