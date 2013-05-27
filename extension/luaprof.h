#ifndef _LUAPROF_H_
#define _LUAPROF_H_

#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdarg.h>
#include"lua.h"
#include"lauxlib.h"
#include"common.h"


/*the gc memory size*/
#define MSIZE 1000
#define LUAPROF_DEBUG

typedef struct Func{
    char *func_name;    /* function name */
    char *source;       /* source of function, the file it defined */
    char *type;         /* type of function;Lua | C | main */
    unsigned int line;           /* the line number of the function defined */
    unsigned int cline;         /* current line, the linenumber function be called */
    int count;          /* count that the function has been called, -1 for unknown */
    unsigned int recursive;       /* deep of recursive call */
    unsigned long total;          /* time of the function total cost */
    unsigned long time;           /* net time of the function cost */
    unsigned long net_begin;
    unsigned long net_end;
    unsigned long begin;
    unsigned long end;
    unsigned int index;  /*index the function in the tree*/
} Func;

typedef struct FuncNode{
    struct FuncNode *pre;
    struct FuncNode *next;
    Func *item;
} FuncNode;

typedef struct Mem { 
    void *table[MSIZE];
    unsigned int n;
} Mem;


void pf_hook(lua_State *L, lua_Debug *ar);
int pf_call(lua_Debug *debug);
int pf_ret(lua_Debug *debug);
int pf_start(lua_State *L);
int pf_stop(lua_State *L);
int pf_save2dot(lua_State *L);
int pf_save2js(lua_State *L);
int pf_save2txt(lua_State *L);
int pf_printr(lua_State *L);
int pf_test();
int pf_release();


#endif
