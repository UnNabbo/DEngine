#include "system.h"

#include "defines.h"

#include <windows.h>

internal SYSTEM_INFO SystemInfo = {};

void RetriveSystemInfo(){
    GetSystemInfo(&SystemInfo);
}

size_t GetPageSize(){
    return SystemInfo.dwPageSize;
}
