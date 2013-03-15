#ifndef _LUAPROF_H_
#define _LUAPROF_H_

#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include"lua.h"
#include"lauxlib.h"
#include"common.h"

typedef struct Func{
    char *func_name;    /*function name*/
    char *source;       /*source of function, the file it defined*/
    char *type;         /*type of function;Lua | C | main*/
    int line;           /*the line number of the function defined*/
    int count;          /*count that the function has been called*/
    int recursive;       /*deep of recursive call*/
    long total;          /*time of the function total cost*/
    long time;           /*net time of the function cost*/
    long net_begin;
    long net_end;
    long begin;
    long end;
    int index;  /*index the function in the tree*/
} Func;

typedef struct FuncNode{
    struct FuncNode *pre;
    struct FuncNode *next;
    Func *item;
} FuncNode;

typedef struct mem{ 
    void *table[10000];
    int n;
} mem;

char* error;
mem gc;

/*The stack*/

void pf_hook(lua_State *L, lua_Debug *ar);
int pf_call(lua_Debug *debug);
int pf_ret(lua_Debug *debug);
int pf_start(lua_State *L);
int pf_stop(lua_State *L);
int pf_output(lua_State *L);
Func* newFunc();


#endif
