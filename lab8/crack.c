#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <crypt.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_LINE 256
char *dict_mem = NULL;
size_t dict_size = 0;
char **lines = NULL;
volatile long total_lines = 0;
volatile long processed_lines=0;
volatile int found=0;
char found_password[MAX_LINE];
volatile long current_line_index=0;
pthread_mutex_t index_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t progress_mutex = PTHREAD_MUTEX_INITIALIZER;
char *target_hash;
char salt_format[256];


void *crack_thread(void *arg) {
    while (1){
pthread_mutex_lock(&index_mutex);
        if (current_line_index >= total_lines) {
            pthread_mutex_unlock(&index_mutex);
            break;}
        long idx = current_line_index;
        current_line_index++;
        pthread_mutex_unlock(&index_mutex);
	if (found)break;

char *word =lines[idx];
word[strcspn(word,"\n")]='\0';
	struct crypt_data data;
        data.initialized = 0;
        char *hash = crypt_r(word, salt_format, &data);
        if (!hash)
            continue;
        if (strcmp(hash, target_hash) == 0) {
            pthread_mutex_lock(&progress_mutex);
            found = 1;
            strncpy(found_password, word, MAX_LINE);
            pthread_mutex_unlock(&progress_mutex);
            break;
        }

        pthread_mutex_lock(&progress_mutex);
        processed_lines++;
        pthread_mutex_unlock(&progress_mutex);
    }
    return NULL;}
int main(int argc, char *argv[]) {
if (argc < 3) {
        fprintf(stderr, "HELP %s <target_hash> <dictionary_file> [thread_count]\n", argv[0]);
        return 1;}
    target_hash = argv[1];
    char *dict_filename = argv[2];
int fd = open(dict_filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;}
struct stat st;
    	if (fstat(fd, &st) < 0) {
        perror("fstat");
        close(fd);
        return 1;}
dict_size = st.st_size;


dict_mem = mmap(NULL, dict_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (dict_mem == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;}
close(fd);

total_lines = 0;
    for(size_t i = 0; i < dict_size; i++) {
        if (dict_mem[i] == '\n')
            total_lines++;}
if (dict_size > 0 && dict_mem[dict_size - 1] != '\n') {
    total_lines++;}
lines =malloc(total_lines * sizeof(char *));
    if (!lines) {
        perror("malloc lines");
        munmap(dict_mem, dict_size);
        return 1;}
long line_index = 0;
lines[line_index++] = dict_mem;
    for (size_t i = 0; i < dict_size; i++) {
        if (dict_mem[i] == '\n') {dict_mem[i] = '\0';

	if (i + 1 < dict_size) {
                lines[line_index++] = &dict_mem[i+1];}}}


char *p = target_hash;
    int dollar_count = 0;
    size_t salt_len = 0;
    while (*p && dollar_count < 3) {
        if (*p == '$')
            dollar_count++;p++;}

   salt_len = p - target_hash;
    if (salt_len >= sizeof(salt_format))
        salt_len = sizeof(salt_format) - 1;
    strncpy(salt_format, target_hash, salt_len);
    salt_format[salt_len] = '\0';
 int thread_count = sysconf(_SC_NPROCESSORS_ONLN);
    if (argc >= 4) {
        thread_count = atoi(argv[3]);
        if (thread_count < 1)thread_count = 1;}
pthread_t *threads = malloc(thread_count * sizeof(pthread_t));
    if (!threads) {
        perror("malloc threads");
        free(lines);
        munmap(dict_mem, dict_size);return 1;}


	for (int i = 0; i < thread_count; i++) {
        if (pthread_create(&threads[i], NULL, crack_thread, NULL) != 0) {
            perror("pthread_create");
            free(threads);
            free(lines);
            munmap(dict_mem, dict_size);
            return 1;}}
 while (!found) {
        pthread_mutex_lock(&progress_mutex);
        double progress = (double)processed_lines / total_lines * 100.0;
        pthread_mutex_unlock(&progress_mutex);
        printf("Progress: %.2f%%\r", progress);
        fflush(stdout);
        if (processed_lines >= total_lines)
            break;usleep(500000);}printf("\n");
for (int i = 0; i < thread_count; i++) {pthread_join(threads[i], NULL);}
if (found) {
        printf("PASS: %s\n", found_password);
    }else{printf("PASS not found\n");}
 free(threads);
    free(lines);
    munmap(dict_mem, dict_size);
 return 0;}
