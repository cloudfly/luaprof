#include"data.h"

void data2dot(tree* t) {
    unsigned int i;
    child *cld;
    char *fpath = "/tmp/.dotfile";
    char *cmd;
    FILE *fp = fopen(fpath, "w+");

    if ( ! fp) {
        printf("[ERROR]: can not create file\n");
        exit(0);
    }
    fprintf(fp, "digraph luaprof {\n");
    for(i = 0;i < t->nfunc;i++) {
        fprintf(fp, "    A%d [label=\"%s\"];\n", i, t->table[i]->item->func_name);
        cld = t->table[i]->children;
        while(cld) {
            fprintf(fp, "    A%d -> A%d [label=\"%d times\"];\n", i ,cld->index, cld->count);
            cld = cld->next;
        }
    }
    fprintf(fp, "}\n");
    fclose(fp);

    cmd = (char*)malloc(sizeof(char) * (24 + strlen(fpath)));
    sprintf(cmd, "dot -Tpng -o graph.png %s", fpath);
    if(system(cmd) == -1) {
        printf("Failed to generate graph\n");
        exit(0);
    }
}
