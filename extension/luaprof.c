#include"luaprof.h"
#include"tree.h"
#include"data.h"


#define checkStack(st, m) (((st) && sameName((st)->item->func_name, (m))) ? 1 : 0)

extern char* error;
Mem gc;
tree* t;
FuncNode* state = (FuncNode*)NULL;


static void pushFunc(Func *item) {

    FuncNode* tmp;

    if (state) {

        state->item->net_end = gettime();
        state->item->time += state->item->net_end - state->item->net_begin; /*save the time data of the stack's top function*/

        tmp = illoc(FuncNode, 1);
        tmp->item = item;
        tmp->pre = state;
        tmp->next = (FuncNode*)NULL;
        state->next = tmp;

        if (sameName(state->item->func_name, item->func_name))   /*if is recursive call, add the recursion's depth*/
            tmp->item->recursive = state->item->recursive + 1;

        state = tmp;

    } else {

        state = illoc(FuncNode, 1);
        state->pre = (FuncNode*)NULL;
        state->next = (FuncNode*)NULL;
        state->item = item;
    }

}

static Func* popFunc() {
    int now = gettime();
    Func* tmp;
    int p = -1; /* index of now poping function's parent */

    if (state) {
        tmp = state->item;
        tmp->net_end = tmp->end = now;
        tmp->time += tmp->net_end - tmp->net_begin;
        tmp->total += tmp->end - tmp->begin;

        state = state->pre;

        if (state) {
            state->next = (FuncNode*)NULL;
            state->item->net_begin = now;   /*reset the net-time's begin data of the stack-top function*/
            p = state->item->index;
        }

        /*give running data to tree*/
        update_time(t, tmp->index, tmp); 
        add_log(t, tmp->index, p, tmp->end - tmp->begin);

        return tmp;

    } else {
        return (Func*)NULL;
    }
}


/*add the function into data array, cld : whether this func is a child, every function is child except main()*/
static void recordFunc(Func* item, int cld){
    Func *res, *prt;
    char* str;

    if (item->recursive > 0) {
        /*sprintf function can get the Bit count of a integer. use stdin to prevent printing content on the screen*/
        str = illoc(char, strlen(item->func_name) + sprintf((char*)stdin, "%d", item->recursive) + 2);
        sprintf(str, "%s@%d", item->func_name, item->recursive);
    } else {
        str = illoc(char, strlen(item->func_name));
        sprintf(str, "%s", item->func_name);
    }

    /*get func in tree.*/
    res = get_func(t, str);

    /* new func */
    if ( ! res) {
        item->func_name = str;
        add_func(t, item);
    } else {
        /*old func*/
        /*
         * `item` has been in tree, 
         * give its data, let it compute based on these data.
         * because now `item` and `res` are the same function.
         */
        item->count = ++(res->count);
        item->time = res->time;
        item->total = res->total;
        item->index = res->index;
    }

    /* the func being called, add it to children. */
    if (cld) {
        /* now this func is on stack top, its parent is down item
         * so, it's state->pre
         */
        prt = get_func(t, state->pre->item->func_name);
        add_cld(t, prt->index, item->index);
    }
}

static Func* newFunc(){
    Func* f;

    f = illoc(Func, 1);
    f->count = 1;
    f->line = -1;
    f->recursive = 0;
    f->time = f->total = 0;
    f->index = -1;

    return f;
}


void pf_hook(lua_State *L, lua_Debug *ar)
{

    lua_getinfo(L, "nlS", ar);

    switch(ar->event) {
    case LUA_HOOKCALL:
        pf_call(ar);
            break;
    case LUA_HOOKRET:
    case LUA_HOOKTAILRET:
        pf_ret(ar);
            break;
    default:
            break;
    }
}

int pf_call(lua_Debug *debug) 
{

    Func *res;

    if ( ! debug || ! debug->name || *debug->what == 'C') return 0;

    res = newFunc();

    res->func_name = illoc(char, strlen(debug->name));
    res->source = illoc(char, strlen(debug->short_src));
    res->type = illoc(char, strlen(debug->what));
    strcpy(res->func_name, debug->name);
    strcpy(res->source, debug->short_src);
    strcpy(res->type, debug->what);

    res->line = debug->linedefined;

    res->begin = res->net_begin = gettime();

    /*can not change the order of two line below, pushFunc can set the recursive to res, then recordFunc use recursive to record function*/
    pushFunc(res);
    recordFunc(res, 1);
    return 1;
}

int pf_ret(lua_Debug *debug)
{

    /**/
    if ( ! debug || *debug->what == 'C') return 0;
    /*if ( ! (debug && debug->name && checkStack(state, debug->name)) ) return 0;   *//*check if the stack top have the function named debug->name */

    popFunc();

    return 1;
}

int pf_start(lua_State *L)
{
    Func* _main;
    gc.n = 0;

    t = illoc(tree, 1);
    memset(t->table, 0, sizeof(t->table));
    t->nfunc = 0;
    lua_sethook(L, (lua_Hook)pf_hook, LUA_MASKCALL | LUA_MASKRET, 0);

    _main = newFunc();

    _main->func_name = illoc(char, 7);
    _main->source = (char *)NULL;
    _main->type = (char *)NULL;
    strcpy(_main->func_name, "main()");

    _main->net_begin = _main->begin = gettime();

    pushFunc(_main);
    recordFunc(_main, 0);
    return 0;
}

int pf_stop(lua_State *L)
{
    Func* item;

    item = popFunc();

    while(item) {
        item = popFunc();
    }

    lua_sethook(L, NULL, 0, 0);
    return 0;
}

int pf_save2dot(lua_State *L) {
    const char* fdot = luaL_checkstring(L, 1);
    char *fpng;

    if (lua_isstring(L, 2)) {
        fpng = (char*)luaL_checkstring(L, 2);
    } else {
        fpng = (char*)NULL;
    }

    if ( ! data2dot(t, fdot, (const char*)fpng)) {
        luaL_error(L, "%s\n", error);
    }
    return 0;
}
int pf_save2js(lua_State *L) {
    const char* filename = luaL_checkstring(L, 1);

    if ( ! data2js(t, filename)) {
        luaL_error(L, "data2js, %s\n", error);
    }
    return 0;
}
int pf_save2txt(lua_State *L) {
    const char* filename = luaL_checkstring(L, 1);

    if ( ! data2text(t, filename)) {
        luaL_error(L, "data2text, %s\n", error);
    }
    return 0;
}

int pf_printr(lua_State *L) {

    unsigned int i = 0;
    FuncNode *slist = sort(t);
    FuncNode *tmp = slist;
    Func *f;

    unsigned int count = (unsigned int)(lua_isnumber(L, 1)?luaL_checknumber(L, 1):SCOUNT);

    if (count > t->nfunc) count = t->nfunc;

    while(tmp) {
        f = tmp->item;
        printf("%-32s%-10d%-15ld%-4.2f%%  %-15ld%.2f%%   [%s]\n", 
                f->func_name, 
                f->count, 
                f->time, 
                f->time / (double)fcvalue(0)->total * 100, 
                f->total, 
                f->total / (double)fcvalue(0)->total * 100, 
                f->source);
        if(++i == count) break;
        tmp = tmp->next;
    }

    printf("\nTotal Time : %ld\n", slist->item->total);

    return 0;
}

int pf_test() {
    
    unsigned int i;
    int j;
    for(i = 0; i < t->nfunc; i++) {
        printf("%s:\n", t->table[i]->item->func_name);
        for(j = 0; j < t->table[i]->item->count; j++) {
            printf("    %u, %u\n", t->table[i]->logs[j].p,t->table[i]->logs[j].time);
        }
    }
    return 0;
}

int pf_release() {

    unsigned int i;

    for(i = 0;i < gc.n; i++) {

        if (gc.table[i]) {
            free(gc.table[i]);
            gc.table[i] = NULL;
        }
    }
    return 0;
}


static const struct luaL_Reg lib[] = {
    {"start", pf_start},
    {"stop", pf_stop},
    {"save2dot", pf_save2dot},
    {"save2js", pf_save2js},
    {"save2txt", pf_save2txt},
    {"printr", pf_printr},
    {"release", pf_release},
    {"test", pf_test},
    {NULL, NULL}
};


int luaopen_luaprof(lua_State *L)
{
    luaL_register(L, "luaprof", lib);

    return 1;
}

