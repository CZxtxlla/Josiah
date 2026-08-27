#ifndef UTILS_H
#define UTILS_H

#include <sys/time.h>

static inline long long getTimeMS() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000LL) + (tv.tv_usec / 1000LL);
}


#endif