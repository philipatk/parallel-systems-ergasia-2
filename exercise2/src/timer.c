#include <stdio.h>
#include <sys/time.h>

double getTime(void) {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec / 1e6;
}