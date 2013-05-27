#include"tree.h"
#include<string.h>


/*
 * find function by name in tree
 */
Func* get_func(tree* t, const char* name){
    unsigned int i;

    
#ifdef LUAPROF_DEBUG
printf("%-20s%-20s\n", "get_func", name);
#endif
    for(i = 0; i < t->nfunc; i++) {
        if (strcmp(name, fcvalue(i)->func_name) == 0) {
            return fcvalue(i);
        }
    }

    return (Func*)NULL;
}

/*add function, return index of new func*/
void add_func(tree* t, Func* f){

    FuncTreeNode* nnode = illoc(FuncTreeNode, 1);

#ifdef LUAPROF_DEBUG
printf("%-20s%-20sPosition %d\n", "add_func", f->func_name, t->nfunc);
#endif

    f->index = t->nfunc;
    nnode->item = f;
    nnode->children = (child*)NULL;
    t->table[t->nfunc++] = nnode;
}

int add_cld(tree* t, int idx, int cidx) {
    FuncTreeNode* f = t->table[idx];
    child* iter = f->children;

#ifdef LUAPROF_DEBUG
printf("%-20s%-20s%-20s\n", "add_cld", fcvalue(idx)->func_name, fcvalue(cidx)->func_name);
#endif

    while(iter) {

        if (iter->index == (unsigned int)cidx) {
            iter->count++;
            return 1;
        }
        iter = iter->next;
    }

    /*child not exist, create new one */
    iter = illoc(child, 1);
    iter->index = cidx; iter->count = 1; iter->next = f->children;
    f->children = iter;
    return 0;
}

void add_log(tree*t, unsigned int idx, int p, unsigned int time) {

    FuncTreeNode *n;
    Log *l;
    int i, j;

    if (idx < t->nfunc) {
        n = t->table[idx];
        i = n->item->count - 1;

        if (i < 20) {
            l = &(n->logs[i]);
            l->p = p;
            l->time = time;
        } else {
            l = &(n->logs[0]);

            for (j = 1; j < LOGCOUNT; j++) {

                if (n->logs[j].time < l->time) {
                    l = &(n->logs[j]);
                }
            }
            l->p = p;
            l->time = time;
        }
    }
}

void update_time(tree* t, int idx, Func* f) {

#ifdef LUAPROF_DEBUG
printf("%-20s%-20sPosition %lu\n", "update_time", f->func_name, f->total);
#endif
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

    return array;
}
