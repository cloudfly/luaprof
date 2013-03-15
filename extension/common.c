
#include"common.h"

int sameName(const char* a, const char* b) {
    int a_len = strlen(a);
    int b_len = strlen(b);

    if (a_len == b_len) return strcmp(a, b) == 0;

    else if (a_len > b_len) return strncmp(a, b, b_len) == 0 && a[b_len] == '@';

    else return strncmp(a, b, a_len) == 0 && b[a_len] == '@';
}

/* get current system time */
long gettime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_usec;
}
