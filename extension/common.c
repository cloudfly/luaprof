#include"common.h"
#include"luaprof.h"

extern Mem gc;

unsigned int sameName(const char* a, const char* b) {
    int a_len = strlen(a);
    int b_len = strlen(b);

    if (a_len == b_len) return strcmp(a, b) == 0;

    else if (a_len > b_len) return strncmp(a, b, b_len) == 0 && a[b_len] == '@';

    else return strncmp(a, b, a_len) == 0 && b[a_len] == '@';
}

/* get current system time */
unsigned long gettime()
{
    /*crossing second may happen between twice calling*/
    static unsigned long isec = 0;

    struct timeval tv;

    gettimeofday(&tv, NULL);

    if(!isec) {
        isec = (unsigned long)tv.tv_sec;
    }

    return (unsigned long)tv.tv_usec + ((unsigned long)tv.tv_sec - isec)*1000000;

}

void* lloc(int size) {
    gc.table[gc.n] = malloc(size);

    if ( ! gc.table[gc.n]) {
        perror("memory not enough\n");
        exit(0);
    }
    memset(gc.table[gc.n], 0, size);
    return gc.table[gc.n++];

}
