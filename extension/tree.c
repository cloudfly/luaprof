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

/*add function, return index of new func*/
void add_func(tree* t, Func* f){

    FuncTreeNode* nnode = (FuncTreeNode*)lloc(sizeof(FuncTreeNode));
    f->index = t->nfunc;
    nnode->item = f;
    nnode->children = (child*)NULL;
    t->table[t->nfunc++] = nnode;
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
    iter = (child*)lloc(sizeof(child));
    iter->index = cidx; iter->count = 1; iter->next = f->children;
    f->children = iter;
    return 0;
}

void update_time(tree* t, int idx, Func* f) {

    if (idx >= 0 && idx < (int)t->nfunc) {
        t->table[idx]->item->time = f->time;
        t->table[idx]->item->total = f->total;
    }
}
