#pragma once

#include "defines.h"


#include "math.h"

#include <emmintrin.h>
#include <xmmintrin.h>
#include <mmintrin.h>
#include <xutility>

#define PI 3.14159265359

union vec2{
    struct
    {
        float x, y;
    };

    struct
    {
        float u, v;
    };

    struct
    {
        float Width, Height;
    };



    alignas(16) __m64 data;
};



union ivec2{
    struct
    {
        int32 x, y;
    };

    struct
    {
        int32 u, v;
    };

    struct
    {
        int32 Width, Height;
    };
    
    alignas(16) __m128i data;


    ivec2 operator+(ivec2& other){
        __m128i Result = _mm_add_epi32(data, other.data);
        return ivec2{.data = Result};
    }

    ivec2 operator-(ivec2& other){
        __m128i Result = _mm_sub_epi32(data, other.data);
        return ivec2{.data = Result};
    }

};


union vec3{
    struct
    {
        float x, y, z;
    };

    struct
    {
        float u, v, w;
    };

    struct
    {
        float Width, Height, Depth;
    };


    struct
    {
        float r, g, b;
    };

    alignas(16) __m128 data;


    operator vec2(){
        return vec2{x, y} ;
    }
};

union ivec3{
    struct
    {
        int32 x, y, z;
    };

    struct
    {
        int32 u, v, w;
    };

    struct
    {
        int32 Width, Height, Depth;
    };


    struct
    {
        int32 r, g, b;
    };

    alignas(16) __m128 data;


    operator ivec2(){
        return ivec2{x, y} ;
    }
};
