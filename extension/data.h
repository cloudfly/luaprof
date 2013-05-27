#ifndef _DATA_H_
#define _DATA_H_

#include"tree.h"

int data2dot(tree* t, const char *fdot, const char *fpng);
int data2text(tree* t, const char* fpath);
int data2js(tree* t, const char* fpath);

#define CMD_PNG "dot -Tpng -o %s %s"
#define openfile(f, s) {umask(S_IXUSR | S_IXGRP | S_IXOTH); \
                        f = fopen((s), "w+"); }
    

#endif
