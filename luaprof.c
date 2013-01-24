#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/time.h>
#include<string.h>
#include"lua.h"
#include"lauxlib.h"


#define MAX 1000

typedef struct {
    char *func_name;    /*function name*/
    char *source;       /*source of function, the file it defined*/
    char *type;         /*type of function;Lua | C | main*/
    int line;           /*the line number of the function defined*/
    int count;          /*count that the function has been called*/
    long time;          /*time of the function total use*/
    long begin;
} func;

func *funcs[MAX]; 
int nfunc = 0;   /* 函数的个数*/
long global_start_time, global_end_time;
const char *filename;

func* getfunc(const char *name);
long pf_gettime();
void pf_hook(lua_State *L, lua_Debug *ar);
int pf_call(lua_Debug *debug);
int pf_ret(lua_Debug *debug);
int pf_start(lua_State *L);
int pf_stop(lua_State *L);
int pf_output();


/* get the function named name from the array*/
func* getfunc(const char *name)
{
    int i;

    for (i = 0;i < nfunc; i++) {
        if (funcs[i] && strcmp(funcs[i]->func_name, name) == 0){
            return funcs[i];
        }
    }
    return (func*)0;
}

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

    func *res;
    if ( ! debug || ! debug->name) return 0;
    res = getfunc(debug->name);

    if (*(debug->what) == 'C') return 0; /*ignore the C function*/

    if ( ! res) {
        res = funcs[nfunc++] = (func*)malloc(sizeof(func));
        res->func_name = (char *)malloc(strlen(debug->name));
        res->source = (char *)malloc(strlen(debug->short_src));
        res->type = (char *)malloc(strlen(debug->what));
        strcpy(res->func_name, debug->name);
        strcpy(res->source, debug->short_src);
        strcpy(res->type, debug->what);
        res->line = debug->linedefined;
        res->count= 1;
        res->time = 0;
    } else {
        res->count++;
    }
    res->begin = pf_gettime();
    return 1;
}

int pf_ret(lua_Debug *debug)
{
    long now = pf_gettime();
    func *res;

    if ( ! debug || ! debug->name) return 0;

    res = getfunc(debug->name);

    if(!res) return 0;

    res->time += (now - res->begin);

    return 1;
}

int pf_start(lua_State *L)
{
    /*set the output file*/
    filename = luaL_checkstring(L, 1);

    memset(funcs, 0, MAX);
    lua_sethook(L, (lua_Hook)pf_hook, LUA_MASKCALL | LUA_MASKRET, 0);
    global_start_time = pf_gettime();
    return 0;
}

int pf_stop(lua_State *L)
{
    global_end_time = pf_gettime();
    lua_sethook(L, NULL, 0, 0);
    return 0;
}

int pf_output(lua_State *L)
{
    FILE *fp = fopen(filename, "w+");
    int i;
    long global_total_time = global_end_time - global_start_time;

    if ( ! fp)
        luaL_error(L, "lprof error : Can not open output file:%s; ensure the file's limits is 666", filename);


    for(i = 0;i < nfunc;i++) {
        if (funcs[i])
            fprintf(fp, "%-32s%-5d%-10ld%.2f%%   [%s]\n", funcs[i]->func_name, funcs[i]->count, funcs[i]->time, funcs[i]->time / (double)global_total_time * 100, funcs[i]->source);
    }
    fprintf(fp, "\nTotal Time : %ld\n", global_total_time);
    fclose(fp);

    for(i = 0;i < nfunc;i++) free(funcs[i]);
    
    return 0;
}

static const struct luaL_Reg lib[] = {
    {"start", pf_start},
    {"stop", pf_stop},
    {"output", pf_output},
    {NULL, NULL}
};


int luaopen_luaprof(lua_State *L)
{
    luaL_register(L, "luaprof", lib);

    return 1;
}
