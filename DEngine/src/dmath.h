#pragma once

#include "defines.h"


#include "math.h"

#include <emmintrin.h>
#include <xmmintrin.h>
#include <mmintrin.h>
#include <smmintrin.h>
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



    alignas(16) __m128 data;


    vec2 operator+(vec2& other){
        __m128 Result = _mm_add_ps(data, other.data);
        return {.data = Result};
    }

    vec2 operator-(vec2& other){
        __m128 Result = _mm_sub_ps(data, other.data);
        return {.data = Result};
    }
    
    vec2 operator*(vec2& other){
        __m128 Result = _mm_mul_ps(data, other.data);
        return {.data = Result};
    }


    vec2 operator*(float other){
        __m128 mul = _mm_set1_ps(other);
        __m128 Result = _mm_mul_ps(data, mul);
        return {.data = Result};
    }

    vec2 operator=(float other){
        return {.data = _mm_set1_ps(other)};
    }
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
    
    ivec2 operator*(ivec2& other){
        __m128i Result = _mm_mullo_epi32(data, other.data);
        return ivec2{.data = Result};
    }


    ivec2 operator*(int other){
        __m128i mul = _mm_set1_epi32(other);
        __m128i Result = _mm_mullo_epi32(data, mul);
        return ivec2{.data = Result};
    }
    
    ivec2 operator=(int other){
        return {.data = _mm_set1_epi32(other)};
        }
    

};

#define IVEC2(x) {x,x};

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

    float elements[3];

    alignas(16) __m128 data;
    

    operator vec2(){
        return vec2{x, y} ;
    }
    
    inline float &operator[](int index){
        return elements[index];
    }
    
    vec3 operator+(vec3& other){
        __m128 Result = _mm_add_ps(data, other.data);
        return {.data = Result};
    }

    vec3 operator-(vec3& other){
        __m128 Result = _mm_sub_ps(data, other.data);
        return {.data = Result};
    }
    
    vec3 operator*(vec3& other){
        __m128 Result = _mm_mul_ps(data, other.data);
        return {.data = Result};
    }


    vec3 operator*(float other){
        __m128 mul = _mm_set1_ps(other);
        __m128 Result = _mm_mul_ps(data, mul);
        return {.data = Result};
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
    
    int elements[3];

    alignas(16) __m128i data;


    operator ivec2(){
        return ivec2{x, y} ;
    }


    ivec3 operator+(ivec3& other){
        __m128i Result = _mm_add_epi32(data, other.data);
        return {.data = Result};
    }

    ivec3 operator-(ivec3& other){
        __m128i Result = _mm_sub_epi32(data, other.data);
        return {.data = Result};
    }
    
    ivec3 operator*(ivec3& other){
        __m128i Result = _mm_mullo_epi32(data, other.data);
        return {.data = Result};
    }


    ivec3 operator*(int other){
        __m128i mul = _mm_set1_epi32(other);
        __m128i Result = _mm_mullo_epi32(data, mul);
        return {.data = Result};
    }


    inline int &operator[](int index){
        return elements[index];
    }
};
