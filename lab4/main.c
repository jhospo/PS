#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>

//z algo
double timespec_diff_sec(struct timespec *start, struct timespec *end) {
    double diff = (end->tv_sec - start->tv_sec);
    diff += (end->tv_nsec - start->tv_nsec) / 1e9;
    return diff;
}
int main(int argc, char *argv[]) {
	int opt;
	int verbose=0;
	int count= 1;

while ((opt= getopt(argc,argv,"vt:")) != -1) {
        switch(opt) {
            case 'v':
                verbose =1;
                break;
            case 't':
                count= atoi(optarg);break;
            default:
                fprintf(stderr, "Usage: %s [-v] [-t count] <program> [args ...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
if (optind>= argc) {
        fprintf(stderr, "BRAK PROGRAMU!!!!\n");
        exit(EXIT_FAILURE);
    }
    double total_real = 0.0, total_user = 0.0, total_sys = 0.0;


for (int i = 0; i < count; i++) {
        struct timespec start, end;
        struct rusage usage;
        int status;
        clock_gettime(CLOCK_MONOTONIC, &start);

pid_t pid =fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }if (pid== 0) {


	if (!verbose) {
                close(1);
                int h=open("/dev/null", O_WRONLY);
                if (h!=-1){
                    dup2(h,1);
                    close(h);
                }
                close(2);
                int h2=open("/dev/null", O_WRONLY);
                if (h2!=-1) {
                    dup2(h2,2);
                    close(h2);
                }
            }
            execvp(argv[optind],&argv[optind]);
perror("execvp");
            exit(EXIT_FAILURE);
        } else {

if (wait4(pid,&status,0, &usage) == -1) {
                perror("wait4");
                exit(EXIT_FAILURE);
            }
	clock_gettime(CLOCK_MONOTONIC, &end);
            double real_time = timespec_diff_sec(&start, &end);
	            double user_time = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1e6;
	            double sys_time  = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1e6;
	 if (verbose) {
	printf("Run %d: real=%.3f s, user=%.3f s, sys=%.3f s\n", i+1, real_time, user_time, sys_time);
            } else {printf("Run %d: real=%.3f s, user=%.3f s, sys=%.3f s\n", i+1, real_time, user_time, sys_time);}
	total_real += real_time;
            total_user += user_time;
            total_sys  += sys_time;}}
if (count > 1) {
        printf("Average: real=%.3f s, user=%.3f s, sys=%.3f s\n", 
               total_real / count, total_user / count, total_sys / count);}
return 0;
}
