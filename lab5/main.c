#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

int stop = 0;
int child_count = 0;
int max_lifetime;
int wait_interval;
int child_duration = 0;

void sigint_handler(int sig) {
    stop = 1;
}

void sigchld_handler(int sig) {
    child_count--;
}

void sigalrm_handler(int sig) {
    _exit(child_duration);
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("usage: %s -w <wait_interval> -m <max_lifetime>\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'w': wait_interval = atoi(argv[++i]); break;
                case 'm': max_lifetime = atoi(argv[++i]); break;
            }
        }
    }

    if (wait_interval <= 0 || max_lifetime <= 0) {
        printf("values must be positive\n");
        return 1;
    }

    srand(time(NULL));

    signal(SIGCHLD, sigchld_handler);
    signal(SIGINT, sigint_handler);

    while (!stop) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            break;
        }

        if (pid == 0) {
            signal(SIGINT, SIG_IGN);
            signal(SIGALRM, sigalrm_handler);

            int duration = (rand() % max_lifetime) + 1;
            child_duration = duration;

            printf("[ %d ] [ %d ]\n", getpid(), duration);
            alarm(duration);

            unsigned long long f = 1;
            for (unsigned long long i = 1;; i++) f *= i;

            _exit(duration);
        } else {
            child_count++;
        }

        sleep(wait_interval);
    }

    printf("SIGINT, czekam na dzieci...\n");

    while (child_count > 0) sleep(1);

    printf("Dzieci zakonczone!\n");
    return 0;
}
