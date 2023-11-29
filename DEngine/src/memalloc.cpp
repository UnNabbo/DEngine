#include "memalloc.h"

#include <windows.h>
#include <winnt.h>

#include "system.h"

#include "logger.h"
#include "stdio.h"

//BEGIN FREE LIST ALLOCATOR 

#define GetFlag(x)      (*(ChunkSize*)x & 0b111)
#define MaskL1(x)       (x &  1)
#define ZeroALL(x)      (x &= 0)
#define ZeroL3Bits(x)   (x = x >> 3 << 3)
#define HideL3Bits(x)   (x >> 3 << 3)

typedef uint32 ChunkSize;

#ifndef MEMALLOC_POOL
#define MEMALLOC_POOL MB(512)
#endif // !MEMALLOC_POOL

typedef uint32 ChunkSize;

struct Allocator{
    ChunkSize Allocated;
    ChunkSize Size;
    void*  Base;
    void*  ReservedPtr;
    ChunkSize ReservedSize;
};

internal Allocator StaticAllocator = {0, 0, 0, 0, AllignPAGE(MEMALLOC_POOL)};

enum ChunkStates : bool{
    FREE = 0,
    ALLOCATED = 1,
};

INLINE void MarkChunk(Byte State, void * Chunk){
    ChunkSize& Size = *(ChunkSize*)((Byte *)Chunk);
    ZeroL3Bits(Size);
    Size |= State;
    malloc(12);
}

INLINE ChunkSize * MakeMemChunk(void * Ptr, ChunkSize Size, bool State){
    ChunkSize AllignedSize = AllignDWORD(Size);
    ChunkSize* NewChunkBase  = (ChunkSize*)Ptr;
    *NewChunkBase = AllignedSize | State;
    *((ChunkSize*)((Byte *)Ptr + AllignedSize + sizeof(ChunkSize))) = AllignedSize | State;
    return NewChunkBase;
};  


#ifdef MEM_DEBUG
void* ImplMemAlloc(uint32 size)
#else 
void* MemAlloc(uint32 size)
#endif // DEBUG
{
    if(size > StaticAllocator.Size - StaticAllocator.Allocated){
        if(!StaticAllocator.Base){
            StaticAllocator.Size = AllignPAGE(size);
            StaticAllocator.ReservedPtr = (Byte *)VirtualAlloc(0, StaticAllocator.ReservedSize, MEM_RESERVE, PAGE_READWRITE);
            StaticAllocator.Base = (Byte *)VirtualAlloc(StaticAllocator.ReservedPtr, StaticAllocator.Size, MEM_COMMIT, PAGE_READWRITE);
            MakeMemChunk(StaticAllocator.Base, StaticAllocator.Size - 2 * sizeof(ChunkSize), FREE);
        } else{
            if(StaticAllocator.Size + AllignPAGE(size) > StaticAllocator.ReservedSize){
                return nullptr;
            }
            VirtualAlloc((Byte*)StaticAllocator.Base + StaticAllocator.Size, AllignPAGE(size), MEM_COMMIT, PAGE_READWRITE);
            ChunkSize* NewAllocChunk = MakeMemChunk((Byte *)StaticAllocator.Base + StaticAllocator.Size, AllignPAGE(size) - 2 * sizeof(ChunkSize), FREE);

            ChunkSize AllocSize = HideL3Bits(*NewAllocChunk);
            ChunkSize PrevAllocSize = HideL3Bits(*(NewAllocChunk - 1));
            ChunkSize NewAllocSize = AllocSize + PrevAllocSize + 2 * sizeof(ChunkSize);
            ZeroALL(*NewAllocChunk);
            ZeroALL(*(NewAllocChunk - 1));
            *(NewAllocChunk - 2 - PrevAllocSize / sizeof(ChunkSize)) = NewAllocSize;
            *(NewAllocChunk + 1 + AllocSize / sizeof(ChunkSize)) = NewAllocSize;

            StaticAllocator.Size += AllignPAGE(size);
        }
    }

    Byte * CurrentBase = (Byte*)StaticAllocator.Base;
    while(true){
        ChunkSize NextChunk = HideL3Bits(*(ChunkSize*)CurrentBase);
        bool State = GetFlag(CurrentBase);
        if(!State && size <= NextChunk){
            MakeMemChunk(CurrentBase, size, ALLOCATED);
            ChunkSize NewSize = size + 2 * sizeof(ChunkSize);
            if (CurrentBase + NewSize < (Byte*)StaticAllocator.Base + StaticAllocator.Size) {
                if (!*(ChunkSize*)(CurrentBase + NewSize)) {
                    *(ChunkSize*)(CurrentBase + NewSize) = NextChunk - NewSize;
                    *(ChunkSize*)(CurrentBase + NextChunk + sizeof(ChunkSize)) = NextChunk - NewSize;
                }
            }
            StaticAllocator.Allocated += AllignDWORD(size);
            return CurrentBase + sizeof(ChunkSize);
        }
        CurrentBase += NextChunk + 2 * sizeof(ChunkSize);
        if (CurrentBase >= (Byte*)StaticAllocator.Base + StaticAllocator.Size) {
            return nullptr;
        }
    }
    
    return 0;
    
}


#ifdef MEM_DEBUG
void ImplMemFree(void * ptr)
#else 
void MemFree(void * ptr)
#endif 
{
    EXIT_IF_NULL(ptr);
    ChunkSize* BasePtr = (ChunkSize*)ptr;
    

    ChunkSize* Begin =  ((ChunkSize*)BasePtr - 1);

    ChunkSize Size = HideL3Bits(*Begin);
    ChunkSize NewSize = Size; 

    ChunkSize* End =  ((ChunkSize*)BasePtr + Size / sizeof(ChunkSize));
    
    if(BasePtr - 2 > StaticAllocator.Base){
        ChunkSize PreviousChunkSize = (*(BasePtr - 2));


        if(!MaskL1(PreviousChunkSize)){
            ZeroALL(*Begin);
            ZeroALL(*(Begin - 1));
            Begin =  (BasePtr - 3 - HideL3Bits(PreviousChunkSize) / sizeof(ChunkSize));
            NewSize += *Begin + 2 * sizeof(ChunkSize);
        } 
    }

    if((Byte *)(BasePtr + Size * sizeof(ChunkSize) + 1) < (Byte*)StaticAllocator.Base + StaticAllocator.Size){
        ChunkSize NextChunkSize =  *(BasePtr + Size / sizeof(ChunkSize) + 1);
            
        if(!MaskL1(NextChunkSize)){
            ZeroALL(*End);
            ZeroALL(*(End + 1));
            End =  (BasePtr + (Size + HideL3Bits(NextChunkSize)) / sizeof(ChunkSize) + 2);
            NewSize += *End + 2 * sizeof(ChunkSize);
        } 

        *Begin = NewSize;
        *End = NewSize;
        StaticAllocator.Allocated -= AllignDWORD(Size);
    }
}

#ifdef MEM_DEBUG

struct AllocatorInfo{
    uint32 TotalAllocatedMem;
    uint32 Allocations;
    uint32 Frees;
};

global AllocatorInfo DebugInfo = {0};

void* DebugMemAlloc(uint32 size){
    void * ptr = ImplMemAlloc(size);
    if(ptr){
        DebugInfo.TotalAllocatedMem += size;
        DebugInfo.Allocations++;
    }
    return ptr;
}

void DebugMemFree(void * ptr){
    ImplMemFree(ptr);
    DebugInfo.Frees++ ;
}

void DumpMemDebug(){
    Log("\n---------------Memory Info--------------\n"
        "Total Memory Allocated:  %u B\n"
        "Leaked Memory:           %u B\n"
        "Allocations Made:        %u B\n"
        "Frees Made:              %u B\n"
        "Heap Size:               %u B\n"
        "----------------------------------------\n\n",
        DebugInfo.TotalAllocatedMem, StaticAllocator.Allocated, DebugInfo.Allocations, DebugInfo.Frees, StaticAllocator.Size);
}


#endif // DEBUG

//END FREE LIST ALLOCATOR 


