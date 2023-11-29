#pragma once

#include "defines.h"

void RetriveSystemInfo();
size_t GetPageSize();

//NOTE  : CHANGE ALLIGNMENT PRGROMMATICATILY 

#define AllignDWORD(x) ((x + 3) & (-4))

#define AllignPAGE(x) ((x + 4095) & (-4096))
