#pragma once


#define CONCAT_IMPL( x, y ) x##y
#define MACRO_CONCAT( x, y ) CONCAT_IMPL( x, y )

#define Log(a,...) char CONCAT_IMPL(buff, __LINE__)[64]; sprintf(CONCAT_IMPL(buff, __LINE__), a, __VA_ARGS__); OutputDebugStringA(CONCAT_IMPL(buff, __LINE__))
