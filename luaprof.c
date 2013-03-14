#include"luaprof.h"
#include"tree.h"

tree* t;
long global_time;
const char *filename;
FuncNode* state = (FuncNode*)NULL;

void pushFunc(Func *item) {
    FuncNode* tmp;

    if (state) {

        state->item->net_end = gettime();
        state->item->time += state->item->net_end - state->item->net_begin; /*save the time data of the stack's top function*/

        tmp = (FuncNode*)malloc(sizeof(FuncNode));
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
    int now = gettime();
    Func* tmp;

    if (state) {
        tmp = state->item;
        tmp->net_end = tmp->end = now;
        tmp->time += tmp->net_end - tmp->net_begin;
        tmp->total += tmp->end - tmp->begin;
        printf("%s at %d 's time is %ld\n", tmp->func_name, tmp->index,tmp->time);

        state = state->pre;

        if (state) {
            state->next = (FuncNode*)NULL;
            state->item->net_begin = now;   /*reset the net-time's begin data of the stack-top function*/
        }

        /*give running data to tree*/
        update_time(t, tmp->index, tmp);

        return tmp;

    } else {
        return (Func*)NULL;
    }
}


int checkStack(const char* name) {
    return (state && sameName(state->item->func_name, name)) ? 1 : 0 ;
}

/*add the function into data array, cld : whether this func is a child, every function is child except main()*/
void recordFunc(Func* item, int cld){
    Func *res, *prt;
    char* str;

    if (item->recursive > 0) {
        /*sprintf function can get the Bit count of a integer. use stdin to prevent printing content on the screen*/
        str = (char*)malloc(sizeof(char) * (strlen(item->func_name) + sprintf((char*)stdin, "%d", item->recursive) + 2));
        sprintf(str, "%s@%d", item->func_name, item->recursive);
    } else {
        str = (char*)malloc(sizeof(char) * strlen(item->func_name));
        sprintf(str, "%s", item->func_name);
    }

    /*get func in tree.*/
    res = get_func(t, str);

    /* new func */
    if ( ! res) {
        free(item->func_name);
        item->func_name = str;
        add_func(t, item);
    } else {
        /*old func*/
        item->count = ++(res->count);
        item->time = res->time;
        item->total = res->total;
        item->index = res->index;
    }

    /* the func being called, add it to children. */
    if (cld) {
        prt = get_func(t, state->item->func_name);
        add_cld(t, prt->index, item->index);
    }
}

Func* newFunc(){
    Func* f;

    f = (Func*)malloc(sizeof(Func));
    f->count = 1;
    f->recursive = 0;
    f->time = f->total = 0;
    f->index = -1;

    return f;
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

    res->begin = res->net_begin = gettime();

    /*can not change the order of two line below, pushFunc can set the recursive to res, then recordFunc use recursive to record function*/
    pushFunc(res);
    recordFunc(res, 1);
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
    Func* _main;
    /*set the output file*/
    filename = luaL_checkstring(L, 1);

    t = (tree*)malloc(sizeof(tree));
    memset(t->table, 0, sizeof(t->table));
    t->nfunc = 0;
    lua_sethook(L, (lua_Hook)pf_hook, LUA_MASKCALL | LUA_MASKRET, 0);

    _main = newFunc();

    _main->func_name = (char *)malloc(sizeof(char) * 7);
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
        global_time = item->total;
        item = popFunc();
    }

    lua_sethook(L, NULL, 0, 0);
    return 0;
}

int pf_output(lua_State *L)
{
    FILE *fp = fopen(filename, "w+");
    unsigned int i;

    if ( ! fp)
        luaL_error(L, "lprof error : Can not open output file:%s; ensure the file's limits is 666", filename);


    for(i = 0;i < t->nfunc;i++) {
        if (t->table[i])
            fprintf(fp, "%-32s%-10d%-15ld%-4.2f%%  %-15ld%.2f%%   [%s]\n", t->table[i]->item->func_name, t->table[i]->item->count, t->table[i]->item->time, t->table[i]->item->time / (double)global_time * 100, t->table[i]->item->total, t->table[i]->item->total / (double)global_time * 100, t->table[i]->item->source);
    }
    fprintf(fp, "\nTotal Time : %ld\n", global_time);
    fclose(fp);

    for(i = 0;i < t->nfunc;i++) free(t->table[i]);
    
    return 0;
}

int pf_test(lua_State *L){
    char a[] = {'a', '\0', 'b'};
    luaL_Buffer b;
    lua_checkstack(L, 1);
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

