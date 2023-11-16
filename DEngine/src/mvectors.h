#pragma once

#include "defines.h"

#include <emmintrin.h>
#include <vcruntime_new.h>
#include <xmmintrin.h>
#include <mmintrin.h>
#include <smmintrin.h>
#include <xutility>

union vec2;
union vec3;

union ivec2;
union ivec3;

//TODO : MOVE OPERATORS IN CPP FILE 
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



    alignas(16) __m128 _data;

    
    INLINE operator vec3();

    INLINE vec2 operator+(vec2& other){
        __m128 Result = _mm_add_ps(_data, other._data);
        return {._data = Result};
    }

    INLINE vec2 operator-(vec2& other){
        __m128 Result = _mm_sub_ps(_data, other._data);
        return {._data = Result};
    }
    
    INLINE vec2 operator*(vec2& other){
        __m128 Result = _mm_mul_ps(_data, other._data);
        return {._data = Result};
    }


    INLINE vec2 operator*(float other){
        __m128 mul = _mm_set1_ps(other);
        __m128 Result = _mm_mul_ps(_data, mul);
        return {._data = Result};
    }

    INLINE vec2 operator=(float other){
        return {._data = _mm_set1_ps(other)};
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
    
    alignas(16) __m128i _data;
    

    INLINE operator ivec3();

    INLINE ivec2 operator+(ivec2& other){
        __m128i Result = _mm_add_epi32(_data, other._data);
        return ivec2{._data = Result};
    }

    INLINE ivec2 operator-(ivec2& other){
        __m128i Result = _mm_sub_epi32(_data, other._data);
        return ivec2{._data = Result};
    }
    
    INLINE ivec2 operator*(ivec2& other){
        __m128i Result = _mm_mullo_epi32(_data, other._data);
        return ivec2{._data = Result};
    }


    INLINE ivec2 operator*(int other){
        __m128i mul = _mm_set1_epi32(other);
        __m128i Result = _mm_mullo_epi32(_data, mul);
        return ivec2{._data = Result};
    }
    
    INLINE ivec2 operator=(int other){
        return {._data = _mm_set1_epi32(other)};
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

    struct
    { 
        vec2 xy;
    };

    struct
    { 
        vec2 xz;
    };


    struct
    { 
        vec2 yz;
    };

    float Components[3];

    alignas(16) __m128 _data;
    
//OPERATORS;

    INLINE operator vec2();
    
    INLINE float &operator[](int index){
        return Components[index];
    }
    
    INLINE vec3 operator+(vec3& other){
        __m128 Result = _mm_add_ps(_data, other._data);
        return {._data = Result};
    }

    INLINE vec3 operator-(vec3& other){
        __m128 Result = _mm_sub_ps(_data, other._data);
        return {._data = Result};
    }
    
    INLINE vec3 operator*(vec3& other){
        __m128 Result = _mm_mul_ps(_data, other._data);
        return {._data = Result};
    }


    INLINE vec3 operator*(float other){
        __m128 mul = _mm_set1_ps(other);
        __m128 Result = _mm_mul_ps(_data, mul);
        return {._data = Result};
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
        

    struct
    { 
        ivec2 xy;
    };

    struct
    { 
        ivec2 xz;
    };


    struct
    { 
        ivec2 yz;
    };

    int Components[3];

    alignas(16) __m128i _data;

//OPERATORS;

    INLINE operator ivec2();

    INLINE ivec3 operator+(ivec3& other){
        __m128i Result = _mm_add_epi32(_data, other._data);
        return {._data = Result};
    }

    INLINE ivec3 operator-(ivec3& other){
        __m128i Result = _mm_sub_epi32(_data, other._data);
        return {._data = Result};
    }
    
    INLINE ivec3 operator*(ivec3& other){
        __m128i Result = _mm_mullo_epi32(_data, other._data);
        return {._data = Result};
    }

    INLINE ivec3 operator*(int other){
        __m128i mul = _mm_set1_epi32(other);
        __m128i Result = _mm_mullo_epi32(_data, mul);
        return {._data = Result};
    }

    INLINE int &operator[](int index){
        return Components[index];
    }
};


union vec4{
    struct
    {
        float x, y, z, w;
    };

    struct
    {
        float r, g, b, a;
    };

    struct
    { 
        vec2 xy;
    };


    struct
    { 
        vec2 zw;
    };

    float Components[4];

    alignas(16) __m128 _data;
    
//OPERATORS;

    INLINE operator vec2();

    INLINE operator vec3(){
        return vec3{x, y, z} ;
    }
    
    INLINE float &operator[](int index){
        return Components[index];
    }
    
    INLINE vec4 operator+(vec4& other){
        __m128 Result = _mm_add_ps(_data, other._data);
        return {._data = Result};
    }

    INLINE vec4 operator-(vec4& other){
        __m128 Result = _mm_sub_ps(_data, other._data);
        return {._data = Result};
    }
    
    INLINE vec4 operator*(vec4& other){
        __m128 Result = _mm_mul_ps(_data, other._data);
        return {._data = Result};
    }


    INLINE vec4 operator*(float other){
        __m128 mul = _mm_set1_ps(other);
        __m128 Result = _mm_mul_ps(_data, mul);
        return {._data = Result};
    }

};

vec2::operator vec3(){
    return {._data = _data};
}

vec3::operator vec2(){
    return {._data = _data};
}

ivec2::operator ivec3(){
    return {._data = _data};
}


ivec3::operator ivec2(){
    return {._data = _data};
}

INLINE float CrossProduct2D(vec2 a, vec2 b){
    return a.x * b.y - a.y * b.x;
}


INLINE int CrossProduct2D(ivec2 a, ivec2 b){
    return a.x * b.y - a.y * b.x;
}
