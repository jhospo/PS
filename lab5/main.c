#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>
volatile sig_atomic_t stop = 0;
volatile sig_atomic_t child_count = 0;
int max_lifetime; 
int wait_interval;
volatile sig_atomic_t child_duration = 0;
void sigint_handler(int sig) {stop = 1;}


void sigchld_handler(int sig, siginfo_t *info, void *context) {
    time_t now = time(NULL);
    char timestr[64];
    snprintf(timestr, sizeof(timestr),"%s",ctime(&now));
    timestr[strcspn(timestr, "\n")] = '\0';
	printf("[ %d ] [ %d ] [ %s ]\n", info->si_pid, info->si_status, timestr);
    	child_count--;}

void sigalrm_handler(int sig) {_exit(child_duration);}

int main(int argc, char *argv[]) {
    int opt;
while ((opt = getopt(argc, argv, "w:m:")) != -1) {
        switch(opt) {
            case 'w':
                wait_interval = atoi(optarg);
                break;
            case 'm':
                max_lifetime = atoi(optarg);
                break;
            default:
                fprintf(stderr, "usage: %s -w <wait_interval> -m <max_lifetime>\n", argv[0]);
                exit(EXIT_FAILURE);}}

if (wait_interval <= 0 || max_lifetime <= 0) {
        fprintf(stderr, "both must be posi\n");
        exit(EXIT_FAILURE);}
//znowu jak na si xd
srand(time(NULL));

struct sigaction sa_chld;
    memset(&sa_chld,0,sizeof(sa_chld));
    sa_chld.sa_sigaction=sigchld_handler;
    sa_chld.sa_flags=SA_SIGINFO | SA_RESTART;
    sigemptyset(&sa_chld.sa_mask);
    if (sigaction(SIGCHLD, &sa_chld, NULL) == -1) {
        perror("sigaction(SIGCHLD)");
        exit(EXIT_FAILURE);}


struct sigaction sa_int;
memset(&sa_int, 0, sizeof(sa_int));
    sa_int.sa_handler = sigint_handler;
    sa_int.sa_flags = SA_RESTART;
    sigemptyset(&sa_int.sa_mask);
    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("sigaction(SIGINT)");
        exit(EXIT_FAILURE);}

while (!stop) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            break;}if(pid == 0){signal(SIGINT, SIG_IGN);

	struct sigaction sa_alrm;
            memset(&sa_alrm, 0, sizeof(sa_alrm));
            sa_alrm.sa_handler = sigalrm_handler;
            sa_alrm.sa_flags = SA_RESTART;
            sigemptyset(&sa_alrm.sa_mask);
            if (sigaction(SIGALRM, &sa_alrm, NULL) == -1) {
                perror("sigaction(SIGALRM)");
                _exit(EXIT_FAILURE);}
	int duration = (rand() % max_lifetime) + 1;
            child_duration = duration;
	time_t now = time(NULL);
            char *timestr = ctime(&now);
            if (timestr) {
                timestr[strcspn(timestr, "\n")] = '\0';}
	printf("[ %d ] [ %d ] [ %s ]\n", getpid(), duration, timestr);
	fflush(stdout);
	alarm(duration);

	volatile unsigned long long factorial = 1;
            for (unsigned long long i = 1;; i++) {
                factorial *= i;}

	_exit(duration);}else{child_count++;}
	sleep(wait_interval);}


	printf("jest SIGINT, kill dzieci\n");
	 while (child_count > 0) {sleep(1);}

	printf("Dzieci zakonczone!!!\n");
    return 0;}
