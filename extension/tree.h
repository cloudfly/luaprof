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

/*return the index of func in table*/
void add_func(tree* t, Func* item);

int add_cld(tree* t, int idx, int cidx);

void update_time(tree* t, int idx, Func* f);

/*get a func from tree by index*/
#define fcvalue(i) (t->table[(i)]->item)

#endif
