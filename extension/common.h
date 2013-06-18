#ifndef _COMMON_H_
#define _COMMON_H_

#include<stdlib.h>
#include<string.h>
#include<sys/time.h>

unsigned int sameName(const char* a, const char* b);
unsigned long gettime();
void* lloc(int size);


#define illoc(c, s) ( (c*)lloc(sizeof(c)*(s)+1) )


#endif
