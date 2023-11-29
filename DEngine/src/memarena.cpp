#include "memarena.h"

#include <vcruntime_string.h>
#include <windows.h>
#include <winnt.h>

#include "system.h"

#ifdef DO_NOT_ALLIGN
#undef AllignPAGE
#define AllignPAGE(x) x
#endif // DEBUG

MemArena AllocateMemArena(uint32 Size ){
    uint32 ASize = AllignPAGE(Size);
    return {ASize, 0, VirtualAlloc(0, ASize, MEM_COMMIT, PAGE_READWRITE) };
}

void * BumpMemArena(MemArena* Arena, uint32 Size){
    if(Size > Arena->Size - Arena->Allocated){
        //TODO : ASSERT 
        return nullptr;
    }
    Byte * ptr = (Byte*)Arena->Base;
    ptr += Arena->Allocated;
    Arena->Allocated += AllignDWORD(Size);
    return ptr;
}


MemArena AllocateSubMemArena(MemArena* Arena,uint32 Size){
    if(Size > Arena->Size - Arena->Allocated){
        //TODO : ASSERT 
        return {0,0,0};
    }

    Byte * Ptr = (Byte*)Arena->Base;
    Ptr += Arena->Allocated;
    Arena->Allocated += AllignDWORD(Size);
    
    return {Size, 0, Ptr};
}


void FreeMemArena(MemArena* Arena){
    Arena->Allocated = 0;
}


void ReleaseMemArena(MemArena* Arena){
    Arena->Allocated = 0;
}
