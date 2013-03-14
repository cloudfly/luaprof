#include"tree.h"
#include<string.h>


/*
 * find function by name in tree
 */
Func* get_func(tree* t, const char* name){
    unsigned int i;
    
    for(i = 0; i < t->nfunc; i++) {
        if (strcmp(name, t->table[i]->item->func_name) == 0)
            return t->table[i]->item;
    }

    return (Func*)NULL;
}

/*
 * find function by name in tree, and give index to argument dix
 */
Func* get_ifunc(tree* t, const char* name, int* idx) {
    unsigned int i;
    
    for(i = 0; i < t->nfunc; i++) {
        if (strcmp(name, t->table[i]->item->func_name) == 0) {
            *idx = (int)i;
            return t->table[i]->item;
        }
    }

    *idx = -1;
    return (Func*)NULL;

}

/*add function, return index of new func*/
unsigned int add_func(tree* t, Func* f){

    FuncTreeNode* nnode = (FuncTreeNode*)malloc(sizeof(FuncTreeNode));
    nnode->item = f;
    nnode->children = (child*)NULL;
    t->table[t->nfunc++] = nnode;

    return t->nfunc - 1;
}

int add_cld(tree* t, int idx, int cidx) {
    FuncTreeNode* f = t->table[idx];
    child* iter = f->children;

    while(iter) {

        if (iter->index == (unsigned int)cidx) {
            iter->count++;
            return 1;
        }
        iter = iter->next;
    }

    /*child not exist, create new one */
    iter = (child*)malloc(sizeof(child));
    iter->index = cidx; iter->count = 1; iter->next = f->children;
    f->children = iter;
    return 0;
}
