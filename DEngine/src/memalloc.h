#pragma once

#include "defines.h"


#define MEM_DEBUG

#ifdef MEM_DEBUG

#define MemAlloc(size) DebugMemAlloc(size)
#define MemFree(ptr) DebugMemFree(ptr)

void* DebugMemAlloc(uint32 size);
void  DebugMemFree(void * ptr);

void* ImplMemAlloc(uint32 size);
void  ImplMemFree(void * ptr);

void DumpMemDebug();

#else

#define DumpMemDebug() 

void* MemAlloc(uint32 size);
void  MemFree(void * ptr);

#endif
