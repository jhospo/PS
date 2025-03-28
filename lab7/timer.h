#ifndef TIMER_H
#define TIMER_H

#include <pthread.h>

extern pthread_key_t timer_key;

void start_timer(void);
double stop_timer(void);

#endif
