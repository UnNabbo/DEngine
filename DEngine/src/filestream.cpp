#include "filestream.h"

#include "windows.h"

#define FILE_SIZE_CAP MB(512)

File MapFile(char*  path, uint32 flags){
    FileHandle file = {}; 
    file.data = 0;
    file.hFile = 0;
    file.hMap = 0;

    LARGE_INTEGER liFileSize;

    file.hFile = CreateFile(path, 
        GENERIC_READ | GENERIC_WRITE,                          // dwDesiredAccess
        0,                                     // dwShareMode
        NULL,                                  // lpSecurityAttributes
        OPEN_EXISTING,                         // dwCreationDisposition
        FILE_ATTRIBUTE_NORMAL,                 // dwFlagsAndAttributes
        0);                                    // hTemplateFile 

    if (file.hFile == INVALID_HANDLE_VALUE) {
        return File{};
    }

    if (!GetFileSizeEx(file.hFile, &liFileSize)) {
        CloseHandle(file.hFile);
        return File{};
    }

    if (liFileSize.QuadPart == 0  || liFileSize.QuadPart > FILE_SIZE_CAP) {
        CloseHandle(file.hFile);
        return File{};
    }

    file.hMap = CreateFileMapping(
        file.hFile,
        NULL,                          // Mapping attributes
        PAGE_READWRITE,                 // Protection flags
        0,                             // MaximumSizeHigh
        0,                             // MaximumSizeLow
        NULL);                         // Name
    
    if (file.hMap == 0) {
        CloseHandle(file.hFile);
        return File{};
    }

    file.data = MapViewOfFile(
        file.hMap,
        FILE_MAP_ALL_ACCESS,         // dwDesiredAccess
        0,                     // dwFileOffsetHigh
        0,                     // dwFileOffsetLow
        0);                    // dwNumberOfBytesToMap
    
    if (file.data == NULL) {
        CloseHandle(file.hMap);
        CloseHandle(file.hFile);
        return File{};
    }
    
    file.size = liFileSize.QuadPart;

    return file;
}


 void UnmapFile(FileHandle file){
    UnmapViewOfFile(file.data);
    CloseHandle(file.hMap);
    CloseHandle(file.hFile);
}
