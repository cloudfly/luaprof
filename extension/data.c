#include"data.h"

char* error;

int data2dot(tree* t, const char *fdot, const char *fpng) {
    unsigned int i;
    child *cld;
    char cmd[1000];
    FILE *fp;
    Func *f = NULL;
    int r, g;
    double rate;

#ifdef LUAPROF_DEBUG
printf("%-20s%25s%25s\n", "data2dot", fdot, fpng);
#endif

    openfile(fp, fdot);

    if ( ! fp) {
        error = "[ERROR]: can not create file\n";
        return 0;
    }

    fprintf(fp, "digraph luaprof {\n");

    for(i = 0;i < t->nfunc;i++) {
        cld = t->table[i]->children;
        f = fcvalue(i);
        rate = f->total / (float)fcvalue(0)->total;

        if (rate < 0.5) {
            r = 0xff * rate * 2;
            g = 0xcc;
        } else {
            r = 0xff;
            g = 0xcc * (1 - rate) * 2;
        }

        if (cld) {
            fprintf(fp, 
                    "    A%d [label=\"%s %.2f%%\" shape=box style = \"filled, rounded\" color = \"#%02x%02x00\"];\n", 
                    i, 
                    f->func_name, 
                    rate * 100,
                    (int)r,
                    (int)g
                    );
        } else {
            fprintf(fp, "    A%d [label=\"%s %.2f%%\" style = \"filled\" color = \"#%02x%02x00\"];\n", 
                    i, 
                    f->func_name, 
                    rate * 100,
                    (int)r,
                    (int)g
                    );
        }

        while(cld) {
            fprintf(fp, "    A%d -> A%d [label=\"%d calls\"];\n", i ,cld->index, cld->count);
            cld = cld->next;
        }
    }
    fprintf(fp, "}\n");
    fclose(fp);

    if (fpng) {
        sprintf(cmd, CMD_PNG, fpng, fdot);

#ifdef LUAPROF_DEBUG
printf("%-20s%25s\n", "data2dot", "GeneratePngByDot");
#endif
        if(system(cmd) < 0) {
            error = "Failed to generate graph\n";
            return 0;
        }
    }

    return 1;
}

int data2text(tree* t, const char* fpath) {

    unsigned int i;

    FILE *fp;

    Func *f = NULL;

#ifdef LUAPROF_DEBUG
printf("%-20s%-25s\n", "data2text", fpath);
#endif

    openfile(fp, fpath);

    if ( ! fp) {
        error = "lprof error : Can not open output; ensure the file's limits is 666";
        return 0;
    }

    for(i = 0;i < t->nfunc;i++) {

        if (t->table[i])
            f = fcvalue(i);
            fprintf(fp, "%-32s%-10d%-15ld%-4.2f%%  %-15ld%.2f%%   [%s]\n", f->func_name, f->count, f->time, f->time / (double)fcvalue(0)->total * 100, f->total, f->total / (double)fcvalue(0)->total * 100, f->source);
    }

    fprintf(fp, "\nTotal Time : %ld\n", fcvalue(0)->total);
    fclose(fp);
    return 1;
}

int data2js(tree* t, const char* fpath) {
    unsigned int i;

    FILE *fp;

    Func *f = NULL;

#ifdef LUAPROF_DEBUG
printf("%-20s%-25s\n", "data2js", fpath);
#endif

    openfile(fp, fpath);

    if ( ! fp) {
        error = "lprof error : Can not open output file";
        return 0;
    }

    fprintf(fp, "var profile_data = [\n");

    for(i = 0;i < t->nfunc; i++) {

        if (t->table[i]) {
            f = fcvalue(i);
            fprintf(fp, "{name:'%s', source:'%s', type:'%s', line:%d, count:%d, total:%ld, time:%ld },\n", f->func_name, f->source, f->type, f->line, f->count, f->total, f->time);
        }
    }
    fprintf(fp, "];\n");
    fclose(fp);
    return 1;
}
