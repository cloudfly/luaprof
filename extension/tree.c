#include"tree.h"
#include<string.h>


/*
 * find function by name in tree
 */
Func* get_func(tree* t, const char* name){
    unsigned int i;
    
    for(i = 0; i < t->nfunc; i++) {
        if (strcmp(name, fcvalue(i)->func_name) == 0)
            return fcvalue(i);
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
        fcvalue(idx)->time = f->time;
        fcvalue(idx)->total = f->total;
    }
}

FuncNode *sort(tree* t) {
    unsigned int i;
    FuncNode *tmp, *tmp2, *array = illoc(FuncNode, 1);
    array->item = t->table[0]->item;
    array->pre = array->next = NULL;
    printf("in sort function\n");
    
    for(i = 1; i < t->nfunc; i++) {
        tmp = array;

        while(1){

            if (tmp->item->total > fcvalue(i)->total) {

                if (tmp->next) {
                    tmp = tmp->next;
                    continue;
                } else {
                    break;
                }

            } else {
                tmp2 = illoc(FuncNode, 1);
                tmp2->item = fcvalue(i);
                tmp2->next = tmp;
                tmp->pre = tmp->pre;
                tmp->pre->next = tmp2;
                tmp->pre = tmp2; 

                if(tmp == array){
                    array = tmp;
                }
                tmp = tmp2;
                break;
            }
        }
        
        if ( ! tmp->next) {
            tmp2 = illoc(FuncNode, 1);
            tmp2->item = fcvalue(i);
            tmp2->pre = tmp; 
            tmp2->next = NULL;
            tmp->next = tmp2;
        }

    }

    /*test begin*/
    tmp = array;

    while(tmp) {
        printf("%s(%lu)->", tmp->item->func_name, tmp->item->total);
        tmp = tmp->next;
    }
    /*test end*/

    return array;
}
