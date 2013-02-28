#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/time.h>
#include<string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include"lua.h"
#include"lauxlib.h"
#include"luaprof.h"



/*The stack*/
FuncNode* state = (FuncNode*)NULL;

Func *funcs[MAX]; 
int nfunc = 0;       /*函数的个数*/
long global_time;
const char *filename;

Func* getFunc(const char *name);
long pf_gettime();
void pf_hook(lua_State *L, lua_Debug *ar);
int pf_call(lua_Debug *debug);
int pf_ret(lua_Debug *debug);
int pf_start(lua_State *L);
int pf_stop(lua_State *L);
int pf_output(lua_State *L);
int sameName(const char* a, const char* b);
Func* newFunc();


void pushFunc(Func *item) {

    if (state) {

        state->item->net_end = pf_gettime();
        state->item->time += state->item->net_end - state->item->net_begin; /*save the time data of the stack's top function*/

        FuncNode* tmp = (FuncNode*)malloc(sizeof(FuncNode));
        tmp->item = item;
        tmp->pre = state;
        tmp->next = (FuncNode*)NULL;
        state->next = tmp;

        if (sameName(state->item->func_name, item->func_name))   /*if is recursive call, add the recursion's depth*/
            tmp->item->recursive = state->item->recursive + 1;

        state = tmp;
    } else {
        state = (FuncNode*)malloc(sizeof(FuncNode*));
        state->pre = (FuncNode*)NULL;
        state->next = (FuncNode*)NULL;
        state->item = item;
    }
}

Func* popFunc() {
    int now = pf_gettime();

    if (state) {
        Func* tmp = state->item;
        tmp->net_end = tmp->end = now;
        tmp->time += tmp->net_end - tmp->net_begin;
        tmp->total = tmp->end - tmp->begin;

        state = state->pre;

        if (state) {
            state->next = (FuncNode*)NULL;
            state->item->net_begin = now;   /*reset the net-time's begin data of the stack-top function*/
        }

        return tmp;

    } else {
        return (Func*)NULL;
    }
}

int sameName(const char* a, const char* b) {
    int a_len = strlen(a);
    int b_len = strlen(b);

    if (a_len == b_len) return strcmp(a, b) == 0;

    else if (a_len > b_len) return strncmp(a, b, b_len) == 0 && a[b_len] == '@';

    else return strncmp(a, b, a_len) == 0 && b[a_len] == '@';
}

int checkStack(const char* name) {
    return (state && sameName(state->item->func_name, name)) ? 1 : 0 ;
}


/*get the function by name from the list*/
Func* getFunc(const char *name)
{
    int i;

    for (i = 0;i < nfunc; i++) {
        if (funcs[i] && strcmp(funcs[i]->func_name, name) == 0){
            return funcs[i];
        }
    }
    return (Func*)NULL;
}


/*add the function into data array*/
void recordFunc(Func* item){
    Func* res;
    char* str;

    if (item->recursive > 0) {
        /*sprintf function can get the Bit count of a integer. use stdin to prevent printing content on the screen*/
        str = (char*)malloc(sizeof(char) * (strlen(item->func_name) + sprintf((char*)stdin, "%d", item->recursive) + 2));
        sprintf(str, "%s@%d", item->func_name, item->recursive);
    } else {
        str = (char*)malloc(sizeof(char) * strlen(item->func_name));
        sprintf(str, "%s", item->func_name);
    }

    res = getFunc(str);

    if ( ! res) {
        free(item->func_name);
        item->func_name = str;
        funcs[nfunc++] = item;
    } else {
        res->count++;
        res->time += item->time;
        res->total += item->total;
    }
}

Func* newFunc(){
    Func* f;

    f = (Func*)malloc(sizeof(Func));
    f->count = 1;
    f->recursive = 0;
    f->time = f->total = 0;

    return f;
}

/* get current system time */
long pf_gettime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_usec;
}

void pf_hook(lua_State *L, lua_Debug *ar)
{

    lua_getinfo(L, "nS", ar);

    switch(ar->event) {
    case LUA_HOOKCALL:
        pf_call(ar);
            break;
    case LUA_HOOKRET:
        pf_ret(ar);
            break;
    }
}

int pf_call(lua_Debug *debug) 
{

    Func *res;

    if ( ! debug || ! debug->name || *(debug->what) == 'C') return 0;

    res = newFunc();

    res->func_name = (char *)malloc(strlen(debug->name));
    res->source = (char *)malloc(strlen(debug->short_src));
    res->type = (char *)malloc(strlen(debug->what));
    strcpy(res->func_name, debug->name);
    strcpy(res->source, debug->short_src);
    strcpy(res->type, debug->what);

    res->line = debug->linedefined;

    res->begin = res->net_begin = pf_gettime();

    pushFunc(res);
    recordFunc(res);
    return 1;
}

int pf_ret(lua_Debug *debug)
{
    if ( ! debug || ! debug->name) return 0;

    if ( ! checkStack(debug->name)) return 0;   /*check if the stack top have the function named debug->name */

    popFunc();

    return 1;
}

int pf_start(lua_State *L)
{
    Func* main;
    /*set the output file*/
    filename = luaL_checkstring(L, 1);

    memset(funcs, 0, MAX);
    lua_sethook(L, (lua_Hook)pf_hook, LUA_MASKCALL | LUA_MASKRET, 0);

    main = newFunc();

    main->func_name = (char *)malloc(sizeof(char) * 7);
    main->source = (char *)NULL;
    main->type = (char *)NULL;
    strcpy(main->func_name, "main()");

    main->net_begin = main->begin = pf_gettime();

    pushFunc(main);
    recordFunc(main);
    return 0;
}

int pf_stop(lua_State *L)
{
    Func* item;

    item = popFunc();

    while(item) {
        global_time = item->total;
        item = popFunc();
    }

    lua_sethook(L, NULL, 0, 0);
    return 0;
}

int pf_output(lua_State *L)
{
    FILE *fp = fopen(filename, "w+");
    int i;

    if ( ! fp)
        luaL_error(L, "lprof error : Can not open output file:%s; ensure the file's limits is 666", filename);


    for(i = 0;i < nfunc;i++) {
        if (funcs[i])
            fprintf(fp, "%-32s%-10d%-15ld%-4.2f%%  %-15ld%.2f%%   [%s]\n", funcs[i]->func_name, funcs[i]->count, funcs[i]->time, funcs[i]->time / (double)global_time * 100, funcs[i]->total, funcs[i]->total / (double)global_time * 100, funcs[i]->source);
    }
    fprintf(fp, "\nTotal Time : %ld\n", global_time);
    fclose(fp);

    for(i = 0;i < nfunc;i++) free(funcs[i]);
    
    return 0;
}

int pf_test(lua_State *L){
    lua_checkstack(L, 1);
    char a[] = {'a', '\0', 'b'};
    luaL_Buffer b;
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, a, 3);
    luaL_pushresult(&b);
    return 1;
}
int pf_test2(lua_State *L) {
    size_t l;
    const char* str = luaL_checklstring(L, 1, &l);
    printf("the strlen of %s is %d:%d\n", str, (int)strlen(str), (int)l);
    return 0;
}

static const struct luaL_Reg lib[] = {
    {"start", pf_start},
    {"stop", pf_stop},
    {"output", pf_output},
    {"test", pf_test},
    {"test2", pf_test2},
    {NULL, NULL}
};


int luaopen_luaprof(lua_State *L)
{
    luaL_register(L, "luaprof", lib);

    return 1;
}

#ifdef __cplusplus
}
#endif
