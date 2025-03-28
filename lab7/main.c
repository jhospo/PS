#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdbool.h>

typedef struct {
    pthread_t tid;
    int ttl;
    int num;
    bool running;
} thread_data;

thread_data *threads;
int count;

void handler(int sig) {
    printf("[THREAD %lu] got signal %d, time = %.2f ms\n", pthread_self(), sig, stop_timer());
    pthread_exit(0);
}

unsigned long long fact(int n) {
    unsigned long long r = 1;
    for (int i = 2; i <= n; i++) r *= i;
    return r;
}

void *run(void *arg) {
    thread_data *d = (thread_data *)arg;
    signal(SIGUSR1, handler);
    start_timer();
    d->running = true;
    int n = 1;
    while (d->running) {
        unsigned long long f = fact(n);
        if (n % 5 == 0) printf("[THREAD %d] %d! = %llu\n", d->num, n, f);
        n = (n % 20) + 1;
        usleep(100000);
    }
    return NULL;
}

int main(int argc, char **argv) {
    int threads_num = 4, max_ttl = 5;
    int opt;
    while ((opt = getopt(argc, argv, "t:m:")) != -1) {
        if (opt == 't') threads_num = atoi(optarg);
        if (opt == 'm') max_ttl = atoi(optarg);
    }

    threads = malloc(sizeof(thread_data) * threads_num);

    srand(time(NULL));
    for (int i = 0; i < threads_num; i++) {
        threads[i].ttl = (rand() % max_ttl) + 1;
        threads[i].num = i + 1;
        threads[i].running = false;
        pthread_create(&threads[i].tid, NULL, run, &threads[i]);
        printf("[MAIN] thread %d id=%lu ttl=%d\n", i + 1, threads[i].tid, threads[i].ttl);
    }

    struct timeval start;
    gettimeofday(&start, NULL);
    bool *sent = calloc(threads_num, sizeof(bool));

    while (1) {
        struct timeval now;
        gettimeofday(&now, NULL);
        double elapsed = (now.tv_sec - start.tv_sec) + (now.tv_usec - start.tv_usec) / 1e6;

        int done = 0;
        for (int i = 0; i < threads_num; i++) {
            if (!sent[i] && elapsed >= threads[i].ttl) {
                threads[i].running = false;
                pthread_kill(threads[i].tid, SIGUSR1);
                printf("[MAIN] signal sent to thread %d at %.2f s\n", i + 1, elapsed);
                sent[i] = true;
            }
            if (sent[i]) done++;
        }
        if (done == threads_num) break;
        usleep(50000);
    }

    for (int i = 0; i < threads_num; i++) {
        pthread_join(threads[i].tid, NULL);
        printf("[MAIN] thread %d joined\n", i + 1);
    }

    free(threads);
    free(sent);
    return 0;
}
