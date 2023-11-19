#pragma once


#include "defines.h"

enum FileFlags : uint32{
    FILE_READ_TEXT = BIT(1),
    FILE_READ_BINARY = BIT(2),
};

 
struct FileHandle {
    void * data;
    size_t size;

    void* hFile;
    void * hMap;

    operator char*(){
        return (char*)data;
    }
};



typedef FileHandle File;

extern File MapFile(char * name, uint32 flags);
extern void UnmapFile(FileHandle* file);

// extern 
