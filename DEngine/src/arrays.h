#pragma once

#include "defines.h"

#include "memalloc.h"

template<typename T, uint32 Size>
struct Array{
    T* Data;
    T& operator[](uint32 Index) { return Data[Index]; }
    
    Array(){
        
    }
};




