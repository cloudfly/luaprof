#ifndef _TREE_H_
#define _TREE_H_


#include"luaprof.h"

#define MAX 10000
#define SCOUNT 10
#define LOGCOUNT 20

typedef struct child {
    unsigned int index;
    unsigned int count;
    struct child* next;
} child;

typedef struct Log{
    int p;
    unsigned int time;
} Log;

/*tree node, store the function info*/

typedef struct FuncTreeNode {
    Func* item;
    Log logs[LOGCOUNT];
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

void add_log(tree*t, unsigned int idx, int p, unsigned int time);

void update_time(tree* t, int idx, Func* f);

/* 
 * sort the tree by func->total, ignore the parent-child relation
 * return a sorted array, but the array already disordered
 */
/* sort tree by flag */
FuncNode* sort(tree* t);

/*get a func from tree by index*/
#define fcvalue(i) (t->table[(i)]->item)

#endif
