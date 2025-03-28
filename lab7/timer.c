#include "timer.h"
#include <stdlib.h>
#include <sys/time.h>

pthread_key_t timer_key;
static pthread_once_t once = PTHREAD_ONCE_INIT;

static void create_key() {
    pthread_key_create(&timer_key, free);
}

void start_timer() {
    pthread_once(&once, create_key);
    struct timeval *tv = malloc(sizeof(struct timeval));
    gettimeofday(tv, NULL);
    pthread_setspecific(timer_key, tv);
}

double stop_timer() {
    struct timeval now, *start;
    gettimeofday(&now, NULL);
    start = pthread_getspecific(timer_key);
    if (!start) return 0.0;
    double ms = (now.tv_sec - start->tv_sec) * 1000.0;
    ms += (now.tv_usec - start->tv_usec) / 1000.0;
    return ms;
}
