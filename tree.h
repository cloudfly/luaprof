#ifndef _TREE_H_
#define _TREE_H_


#include"luaprof.h"

#define MAX 1000

typedef struct child {
    unsigned int index;
    unsigned int count;
    struct child* next;
} child;

/*tree node, store the function info*/

typedef struct FuncTreeNode {
    Func* item;
    child* children;
} FuncTreeNode;

/*The tree, the array have the tree model*/

typedef struct tree {
    FuncTreeNode* table[MAX];
    unsigned int nfunc;
} tree;

Func* get_func(tree* t, const char* name);
Func* get_ifunc(tree* t, const char* name, int* idx);

/*return the index of func in table*/
unsigned int add_func(tree* t, Func* item);

int add_cld(tree* t, int idx, int cidx);

#endif
