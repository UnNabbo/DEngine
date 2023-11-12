#include "dmath.h"

#include "defines.h"

#include "win32_backend.h"
#include <float.h>
#include <stdio.h>

#define DistanceFN(x1, y1, x2, y2) sqrt( (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) ) 

#define CONCAT_IMPL( x, y ) x##y
#define MACRO_CONCAT( x, y ) CONCAT_IMPL( x, y )
#define Log(a,...) char CONCAT_IMPL(buff, __LINE__)[64]; sprintf(CONCAT_IMPL(buff, __LINE__), a, __VA_ARGS__); OutputDebugStringA(CONCAT_IMPL(buff, __LINE__));

void SetPixel(Bitmap * Buffer,int32 x, int32 y, uint8 r = 0xff, uint8 g = 0x00, uint8 b = 0xff){
    if((x < Buffer->Width && x >= 0) && (y < Buffer->Height && y >= 0)){
        *(((uint32*)Buffer->Memory) + Buffer->Width * y + x ) = b | g << 8 | r << 16;
    }
}


void DrawText(const char * text, size_t len){
    
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

ivec2 Intersect(ivec2 p1, ivec2 p2, ivec2 p3, ivec2 p4) {
    // Store the values for fast access and easy
    // equations-to-code conversion
    float x1 = p1.x, x2 = p2.x, x3 = p3.x, x4 = p4.x;
    float y1 = p1.y, y2 = p2.y, y3 = p3.y, y4 = p4.y;
 
    float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
// If d is zero, there is no intersection
    if (d == 0) return {-1 , -1};
 
// Get the x and y
    float pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
    float x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
    float y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;
 
// Check if the x and y coordinates are within both lines
    if ( x < min(x1, x2) || x > max(x1, x2) ||
    x < min(x3, x4) || x > max(x3, x4) ) return {-1, -1};
    if ( y < min(y1, y2) || y > max(y1, y2) ||
    y < min(y3, y4) || y > max(y3, y4) ) return {-1, -1};
 
// Return the point of intersection
    ivec2 ret ={};
    ret.x = x;
    ret.y = y;
    return ret;
}


typedef int OutCode;

const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000

// Compute the bit code for a point (x, y) using the clip rectangle
// bounded diagonally by (xmin, ymin), and (xmax, ymax)
#define xmin 0
#define ymin 0
#define xmax 255
#define ymax 255
// ASSUME THAT xmax, xmin, ymax and ymin are global constants.

OutCode ComputeOutCode(double x, double y)
{
	OutCode code = INSIDE;  // initialised as being inside of clip window

	if (x < xmin)           // to the left of clip window
		code |= LEFT;
	else if (x > xmax)      // to the right of clip window
		code |= RIGHT;
	if (y < ymin)           // below the clip window
		code |= BOTTOM;
	else if (y > ymax)      // above the clip window
		code |= TOP;

	return code;
}

// Cohen–Sutherland clipping algorithm clips a line from
// P0 = (x0, y0) to P1 = (x1, y1) against a rectangle with 
// diagonal from (xmin, ymin) to (xmax, ymax).
bool CohenSutherlandLineClip(int& x0, int& y0, int& x1, int& y1)
{
	// compute outcodes for P0, P1, and whatever point lies outside the clip rectangle
	OutCode outcode0 = ComputeOutCode(x0, y0);
	OutCode outcode1 = ComputeOutCode(x1, y1);
	bool accept = false;

	while (true) {
		if (!(outcode0 | outcode1)) {
			// bitwise OR is 0: both points inside window; trivially accept and exit loop
			accept = true;
			break;
		} else if (outcode0 & outcode1) {
			// bitwise AND is not 0: both points share an outside zone (LEFT, RIGHT, TOP,
			// or BOTTOM), so both must be outside window; exit loop (accept is false)
			break;
		} else {
    
			// failed both tests, so calculate the line segment to clip
			// from an outside point to an intersection with clip edge
			int x, y;

			// At least one endpoint is outside the clip rectangle; pick it.
			OutCode outcodeOut = outcode1 > outcode0 ? outcode1 : outcode0;

			// Now find the intersection point;
			// use formulas:
			//   slope = (y1 - y0) / (x1 - x0)
			//   x = x0 + (1 / slope) * (ym - y0), where ym is ymin or ymax
			//   y = y0 + slope * (xm - x0), where xm is xmin or xmax
			// No need to worry about divide-by-zero because, in each case, the
			// outcode bit being tested guarantees the denominator is non-zero
     
			if (outcodeOut & TOP) {           // point is above the clip window
				x = x0 + (x1 - x0) * (ymax - y0) / (y1 - y0);
				y = ymax;
			} else if (outcodeOut & BOTTOM) { // point is below the clip window
				x = x0 + (x1 - x0) * (ymin - y0) / (y1 - y0);
				y = ymin;
			} else if (outcodeOut & RIGHT) {  // point is to the right of clip window
				y = y0 + (y1 - y0) * (xmax - x0) / (x1 - x0);
				x = xmax;
			} else if (outcodeOut & LEFT) {   // point is to the left of clip window
				y = y0 + (y1 - y0) * (xmin - x0) / (x1 - x0);
				x = xmin;
			}

			// Now we move outside point to intersection point to clip
			// and get ready for next pass.
			if (outcodeOut == outcode0) {
				x0 = x;
				y0 = y;
				outcode0 = ComputeOutCode(x0, y0);
			} else {
				x1 = x;
				y1 = y;
				outcode1 = ComputeOutCode(x1, y1);
			}
		}
	}
	return accept;
}

void DrawWallB(Bitmap* Buffer, int x1, int x2, int b1, int b2, int t1, int t2, vec3 color = {0xff, 0xff, 0xff}){
   
    int xb1 = x1, xb2 = x2;
    int xt1 = x1, xt2 = x2;

	bool top = CohenSutherlandLineClip(xb1, b1, xb2, b2);
	bool bop = CohenSutherlandLineClip(xt1, t1, xt2, t2);

    Log("(%i, %i) - (%i, %i) \n", xb1, b1, xb2, b2)
    
    

    int DeltaXb = abs(xb2 - xb1);
    int DeltaXt = abs(xt2 - xt1);
    
    int SingX = (x1 < x2) ? 1 : -1;
    int DeltaB = -abs(b2 - b1);
    int SingB = (b1 < b2) ? 1 : -1;
    int DeltaT = -abs(t2 - t1);
    int SingT = (t1 < t2) ? 1 : -1;
    int ErrorB =  DeltaXb + DeltaB;
    int ErrorT =  DeltaXt + DeltaT;
    

    int Xb = xb1;
    int Xt = xt1;
    int B = b1;
    int T = t1;
    

    int SingY = (B < T) ? 1 : -1; 
    while(true){
        SetPixel(Buffer, Xt, T, color.r, color.g, color.b);
        SetPixel(Buffer, Xb, B, color.r, color.g, color.b);

        if(Xb == xb2 && Xt == xt2 && B == b2 && T == t2) break;

        int ErrorB2 = ErrorB * 2;
        int ErrorT2 = ErrorT * 2;


        if(ErrorB2 >= DeltaB){
            if(Xb == xb2) break;
            ErrorB += DeltaB;
            Xb += SingX;
        }


        if(ErrorT2 >= DeltaT){
            if(Xt == xt2) break;
            ErrorT += DeltaT;
            Xt += SingX;
        }

       if(ErrorB2 <= DeltaXb){
            if(B == b2)break;
            ErrorB += DeltaXb;
            B += SingB;
        }

        if(ErrorT2 <= DeltaXb){
            if(T == t2)break;
            ErrorT += DeltaXt;
            T += SingT;
        }
    }
    SetPixel(Buffer, xb1, b1, 0xff);
    SetPixel(Buffer, xb2, b2, 0xff);
}



void DrawLine(Bitmap* Buffer, vec2 Point1, vec2 Point2, vec3 color = {0xff, 0xff, 0xff}){
    DrawLine(Buffer, Point1.x, Point1.y, Point2.x, Point2.y, color);
}

#define DEBUG_LINE(x, y, l) DrawLine(Buffer, x, y, x, y + l, {0xff, 0, 0}); DrawLine(Buffer, x, y, x + l, y , {0, 0xff, 0}); DrawLine(Buffer, x, y, x, y , {0, 0, 0xff})

struct PlayerData{
    vec3 Pos = {0, 0, -64};
    float Yaw = 0;
    float Pitch = 0;
};

PlayerData Player = {};

#define RotatePointX(Pivot, Point, Angle) (Point.x - Pivot.x) * (float)cos(Angle) - (Point.z - Pivot.z) * (float)sin(Angle);

#define RotatePointX2(Pivot, Point, Angle) (Point.x - Pivot.x) * (float)cos(Angle) - (Point.y - Pivot.y) * (float)sin(Angle);

#define RotatePointY(Pivot, Point, Angle) (Point.x - Pivot.x) * (float)sin(Angle) + (Point.z - Pivot.z) * (float)cos(Angle);

#define RotatePointY2(Pivot, Point, Angle) (Point.x - Pivot.x) * (float)sin(Angle) + (Point.y - Pivot.y) * (float)cos(Angle);



vec3 ClipPoint(int x1, int y1, int z1, int x2, int y2, int z2){
    float da = y1;
    float db = y2;
    float d = da - db;
    if(d == 0){
        d = 1;
    }
    float s = da / (da - db);
    vec3 ClippedCords;
    ClippedCords.x = x1 + s * (x2 - x1);
    ClippedCords.y = y1 + s * (y2 - y1);
    if(ClippedCords.y == 0){
        ClippedCords.y = 1;
    }
    ClippedCords.z = z1 + s * (z2 - z1);
    return ClippedCords;
}

vec2 GProjectPoint(Bitmap* Buffer, ivec3 Point, vec3 Pivot, float Angle){
    vec3 RPoint = {};
    RPoint.x =  RotatePointX(Pivot, Point, Angle);
    RPoint.z =  RotatePointY(Pivot, Point, Angle);
    RPoint.y =  0 - Point.y - Pivot.y;
    
    if(!RPoint.z) { RPoint.z = 1; }
    vec2 TPoint = {};
    TPoint.x = RPoint.x * 200.0f / RPoint.z + Buffer->Width / 2;
    TPoint.y = RPoint.y * 200.0f / RPoint.z + Buffer->Height / 2;
    
    return TPoint;
}


vec3 PointToPlayer(Bitmap* Buffer, vec3 Point, vec3 Pivot, float Angle){
    vec3 RPoint = {};
    RPoint.x =  RotatePointX(Pivot, Point, Angle);
    RPoint.z =  RotatePointY(Pivot, Point, Angle);
    RPoint.y =  0 - Point.y - Pivot.y;
    
    return RPoint;
}

void DrawWallI(Bitmap* Buffer, int x1, int x2, int b1, int b2, int t1, int t2, vec3 Color = {0xff, 0xff, 0xff}){
    int dyb = b2 - b1;
    int dyt = t2 - t1;
    int dx =  x2 - x1;
    if(dx == 0){ 
        dx = 1;
    }
    int xs = x1;
    
    if(x1 < 1) { x1 = 1;};
    if(x2 < 1) { x2 = 1;};
    if(x1 > Buffer->Width - 1){ x1 = Buffer->Width - 1;}
    if(x2 > Buffer->Width - 1){ x2 = Buffer->Width - 1;}
    
    for(int x = x1; x <= x2; x++){
        int y1 = dyb * (x-xs) / dx + b1;
        int y2 = dyt * (x-xs) / dx + t1 + 1;

        if(y1 < 1) { x1 = 1;};
        if(y2 < 1) { x2 = 1;};
        if(y1 > Buffer->Height - 1){ y1 = Buffer->Height - 1;}
        if(y2 > Buffer->Height- 1){ y2 = Buffer->Height - 1;}

        for(int y = y1; y<y2; y++){
            SetPixel(Buffer, x, y, Color.r, Color.g, Color.b);
        }
    }
}


void DrawWall(Bitmap* Buffer, int x1, int x2, int b1, int b2, int t1, int t2, vec3 Color = {0xff, 0xff, 0xff}){
    int px1 = x1, px2 = x2, pb1 = b1, pb2 = b2, pt1 = t1, pt2 = t2; 
    if(x1 > x2) {
        px1 = x2;
        px2 = x1;
        pb1 = b2;
        pb2 = b1;
        pt1 = t2;
        pt2 = t1;
    }
    
    DrawWallI(Buffer, px1, px2, pb1, pb2, pt1, pt2, Color);
}


void DrawWallQ(Bitmap* Buffer, vec3 UPoint1, vec3 UPoint2, int Floor, int Celing, vec3 Color = {0xff, 0xff, 0xff}){
    vec3 Point1 = PointToPlayer(Buffer, UPoint1, Player.Pos, Player.Yaw);
    vec3 Point2 = PointToPlayer(Buffer, UPoint2, Player.Pos, Player.Yaw);

    if(Point1.z <= 0 && Point2.z <= 0){
        return;
    }
    
    // TPoint.y = RPoint.y * 200.0f / RPoint.z + Buffer->Height / 2;
    float x1 = (int)(Point1.y * 200.0f / Point1.z) + Buffer->Width / 2;
    float x2 = (int)(Point2.y * 200.0f / Point2.z) + Buffer->Width / 2;
    

    float y1a = (int)((Point1.y + Floor) * 200.0f / Point1.z) + Buffer->Height / 2;
    float y2a = (int)((Point2.y + Floor) * 200.0f / Point2.z) + Buffer->Height / 2;
} 

void GameUpdateAndRender(Bitmap* Buffer){
    ivec2 pos = GetMousePos();
    SetPixel(Buffer, pos.x, pos.y);

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

    if(GetKeydown(VK_SPACE)){
        Player.Pos.y += DeltaMove ;
    }
    

    if(GetKeydown(VK_SHIFT)){
        Player.Pos.y -= DeltaMove ;
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

    ivec3 Center = {0,0,0};
    vec2 TCenter = GProjectPoint(Buffer, Center, Player.Pos, Player.Yaw);
    
    ivec3 Point1 = {-32, 16, 64};
    vec2 TPoint1 = GProjectPoint(Buffer, Point1, Player.Pos, Player.Yaw);
    
    ivec3 Point2 = {32, 16, 64};
    vec2 TPoint2 = GProjectPoint(Buffer, Point2, Player.Pos, Player.Yaw);

    
    ivec3 Point3 = {-32, -16, 64};
    vec2 TPoint3 = GProjectPoint(Buffer, Point3, Player.Pos, Player.Yaw);

    ivec3 Point4 = {32, -16, 64};
    vec2 TPoint4 = GProjectPoint(Buffer, Point4, Player.Pos, Player.Yaw);
    
    
    SetPixel(Buffer, TCenter.x, TCenter.y, 0xff, 0, 0);

    DrawLine(Buffer, TPoint3, TPoint4);
    DrawLine(Buffer, TPoint1, TPoint4);
    DrawLine(Buffer, TPoint2, TPoint4);
    DrawLine(Buffer, TPoint3, TPoint1);
    
    DrawWallB(Buffer, TPoint1.x, TPoint2.x, TPoint1.y, TPoint2.y, TPoint3.y, TPoint4.y, {255,255,0});
    //DrawLine(Buffer, TPoint3, TPoint2);
    


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
