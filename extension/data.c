#include"data.h"

int data2dot(tree* t, const char *fdot, const char *fpng) {
    unsigned int i;
    child *cld;
    char *cmd;
    FILE *fp = fopen(fdot, "w+");

    if ( ! fp) {
        error = "[ERROR]: can not create file\n";
        return 0;
    }

    fprintf(fp, "digraph luaprof {\n");

    for(i = 0;i < t->nfunc;i++) {
        cld = t->table[i]->children;

        if (cld) {
            fprintf(fp, "    A%d [label=\"%s %.2f%%\" shape=box];\n", i, t->table[i]->item->func_name, t->table[i]->item->total / (double)t->table[0]->item->total * 100);
        } else {
            fprintf(fp, "    A%d [label=\"%s %.2f%%\"];\n", i, t->table[i]->item->func_name, t->table[i]->item->total / (double)t->table[0]->item->total * 100);
        }

        while(cld) {
            fprintf(fp, "    A%d -> A%d [label=\"%d calls\"];\n", i ,cld->index, cld->count);
            cld = cld->next;
        }
    }
    fprintf(fp, "}\n");
    fclose(fp);

    if (fpng) {
        cmd = (char*)lloc(sizeof(char) * (15 + strlen(fdot) + strlen(fpng)));
        sprintf(cmd, CMD_PNG, fpng, fdot);

        if(system(cmd) < 0) {
            error = "Failed to generate graph\n";
            free(cmd);
            return 0;
        }
    }

    return 1;
}

int data2text(tree* t, const char* fpath) {

    unsigned int i;

    FILE *fp = fopen(fpath, "w+");

    if ( ! fp) {
        error = "lprof error : Can not open output; ensure the file's limits is 666";
        return 0;
    }

    for(i = 0;i < t->nfunc;i++) {

        if (t->table[i])
            fprintf(fp, "%-32s%-10d%-15ld%-4.2f%%  %-15ld%.2f%%   [%s]\n", t->table[i]->item->func_name, t->table[i]->item->count, t->table[i]->item->time, t->table[i]->item->time / (double)t->table[0]->item->total * 100, t->table[i]->item->total, t->table[i]->item->total / (double)t->table[0]->item->total * 100, t->table[i]->item->source);
    }

    fprintf(fp, "\nTotal Time : %ld\n", t->table[0]->item->total);
    fclose(fp);
    return 1;
}

int data2js(tree* t, const char* fpath) {
    unsigned int i;

    FILE *fp = fopen(fpath, "w+");

    Func *f;

    if ( ! fp) {
        error = "lprof error : Can not open output file";
        return 0;
    }

    fprintf(fp, "var profile_data = [\n");

    for(i = 0;i < t->nfunc; i++) {

        if (t->table[i]) {
            f = t->table[i]->item;
            fprintf(fp, "{name:'%s', source:'%s', type:'%s', line:%d, count:%d, total:%ld, time:%ld },\n", f->func_name, f->source, f->type, f->line, f->count, f->total, f->time);
        }
    }
    fprintf(fp, "];\n");
    fclose(fp);
    return 1;
}

int print_result(tree* t) {

    unsigned int i;

    for(i = 0;i < t->nfunc;i++) {

        if (t->table[i])
            printf("%-32s%-10d%-15ld%-4.2f%%  %-15ld%.2f%%   [%s]\n", t->table[i]->item->func_name, t->table[i]->item->count, t->table[i]->item->time, t->table[i]->item->time / (double)t->table[0]->item->total * 100, t->table[i]->item->total, t->table[i]->item->total / (double)t->table[0]->item->total * 100, t->table[i]->item->source);
    }

    printf("\nTotal Time : %ld\n", t->table[0]->item->total);
    return 1;
}

