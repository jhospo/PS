#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <crypt.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define MAX_LINE 256

char *dict_mem;
size_t dict_size;
char **lines;
long total_lines = 0;
long processed_lines = 0;
long current_line_index = 0;
int found = 0;
char found_password[MAX_LINE];
char *target_hash;
char salt_format[256];

pthread_mutex_t index_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t progress_mutex = PTHREAD_MUTEX_INITIALIZER;

void *crack_thread(void *arg) {
    struct crypt_data data;
    data.initialized = 0;

    while (!found) {
        pthread_mutex_lock(&index_mutex);
        if (current_line_index >= total_lines) {
            pthread_mutex_unlock(&index_mutex);
            break;
        }
        long idx = current_line_index++;
        pthread_mutex_unlock(&index_mutex);

        char *word = lines[idx];
        word[strcspn(word, "\n")] = '\0';

        char *hash = crypt_r(word, salt_format, &data);
        if (hash && strcmp(hash, target_hash) == 0) {
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

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uzycie: %s <hash> <plik> [watki]\n", argv[0]);
        return 1;
    }

    target_hash = argv[1];
    int fd = open(argv[2], O_RDONLY);
    struct stat st;
    fstat(fd, &st);
    dict_size = st.st_size;
    dict_mem = mmap(NULL, dict_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    close(fd);

    for (size_t i = 0; i < dict_size; i++)
        if (dict_mem[i] == '\n') total_lines++;
    if (dict_mem[dict_size - 1] != '\n') total_lines++;

    lines = malloc(total_lines * sizeof(char *));
    long index = 0;
    lines[index++] = dict_mem;
    for (size_t i = 0; i < dict_size; i++) {
        if (dict_mem[i] == '\n') {
            dict_mem[i] = '\0';
            if (i + 1 < dict_size) lines[index++] = &dict_mem[i + 1];
        }
    }

    char *p = target_hash;
    int dol = 0;
    while (*p && dol < 3) if (*p++ == '$') dol++;
    int len = p - target_hash;
    strncpy(salt_format, target_hash, len);
    salt_format[len] = '\0';

    int thread_count = sysconf(_SC_NPROCESSORS_ONLN);
    if (argc >= 4) thread_count = atoi(argv[3]);
    if (thread_count < 1) thread_count = 1;

    pthread_t *threads = malloc(thread_count * sizeof(pthread_t));
    for (int i = 0; i < thread_count; i++)
        pthread_create(&threads[i], NULL, crack_thread, NULL);

    while (!found) {
        double prog;
        pthread_mutex_lock(&progress_mutex);
        prog = (double)processed_lines / total_lines * 100;
        pthread_mutex_unlock(&progress_mutex);
        printf("Postęp: %.2f%%\r", prog);
        fflush(stdout);
        if (processed_lines >= total_lines) break;
        usleep(300000);
    }
    printf("\n");

    for (int i = 0; i < thread_count; i++)
        pthread_join(threads[i], NULL);

    if (found) printf("Znaleziono haslo: %s\n", found_password);
    else printf("Nie znaleziono hasla\n");

    free(threads);
    free(lines);
    munmap(dict_mem, dict_size);
    return 0;
}
