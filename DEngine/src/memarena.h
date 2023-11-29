#pragma once

#include "defines.h"

struct MemArena{
    uint32 Size;
    uint32 Allocated;
    void*  Base;
};

MemArena AllocateMemArena(uint32 Size = 8192);
MemArena AllocateSubMemArena(MemArena* ,uint32 Size = 8192);
void * BumpMemArena(MemArena* Arena, uint32 size); 
void  FreeMemArena(MemArena* Arena); 
void  ReleaseMemArena(MemArena* Arena); 
