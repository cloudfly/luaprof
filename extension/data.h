#ifndef _DATA_H_
#define _DATA_H_

#include"luaprof.h"
#include"tree.h"

int data2dot(tree* t, const char *fdot, const char *fpng);
int data2text(tree* t, const char* fpath);
int data2js(tree* t, const char* fpath);

#endif
