#pragma once

#include "defines.h"

#include <math.h>

#include "mvectors.h"

#define PI 3.14159265359
#undef max
#define max(a,b) (a > b) ? a : b

#undef min
#define min(a,b) (a > b) ? b : a


template<typename T, typename... Args>
constexpr decltype(auto) vmax(T& arg1,Args&... args){
    T arr[] = {args...};
    T maxi = arg1;
    for(auto& item : arr){
        maxi = max(maxi, item);
    }
    return maxi;
}


template<typename T, typename... Args>
constexpr decltype(auto) vmin(T& arg1,Args&... args){
    T arr[] = {args...};
    T mini = arg1;
    for(auto& item : arr){
        mini = min(mini, item);
    }
    return mini;
}

template<typename T>
INLINE void swap(T& a, T& b){
    T _tmp = a;
    a = b;
    b = _tmp;
}
