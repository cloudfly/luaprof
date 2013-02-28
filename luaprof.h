#define MAX 1000

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
} Func;

typedef struct FuncNode{
    struct FuncNode *pre;
    struct FuncNode *next;
    Func *item;
} FuncNode;

/*tree node, store the function info*/
typedef struct FuncTreeNode {
    Func* item;
    /*children is a integer array, one value is the FuncTreeNode'index in the tree, the following value is the FuncTreeNode's calling count
     *
     * eg: children[0] = 5,children[1] = 3. mean this function have called table[5] for 3 times;
     * */

    int* children;
} FuncTreeNode;

/*The tree, the array have the tree model*/
FuncTreeNode* table[MAX];


