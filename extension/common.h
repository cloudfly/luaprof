#ifndef _COMMON_H_
#define _COMMON_H_

#include<stdlib.h>
#include<string.h>
#include<sys/time.h>
#include"luaprof.h"

int sameName(const char* a, const char* b);
long gettime();
void* lloc(int size);

#endif
