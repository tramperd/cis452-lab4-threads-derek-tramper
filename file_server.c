/*
 * Lab 4 – Multi-threaded Programming (pthread)
 * Derek Tramper
 *
 * This program simulates a simple multi-threaded file server.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

int request_count = 0;

volatile sig_atomic_t keep_running = 1;

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;

void sigint_handler(int sig) {
    keep_running = 0;
    printf("\n[Server] Ctrl+C received. Shutting down...\n");
}

void* file_worker(void* arg) {
    char* filename = (char*) arg;

    int chance = rand() % 10;

    if (chance < 8) {
        sleep(1);
    } else {
        int delay = 7 + rand() % 4;
        sleep(delay);
    }

    printf("[Worker] Finished handling file: %s\n", filename);

    free(filename);
    return NULL;
}

int main() {
    signal(SIGINT, sigint_handler);
    srand(time(NULL));

    printf("[Server] File server is running. Press Ctrl+C to stop.\n");

    while (keep_running) {
        char input[256];

        printf("[Dispatcher] Enter a filename: ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            if (feof(stdin)) break;
            continue;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0) continue;

        char* filename_copy = strdup(input);
        if (filename_copy == NULL) {
            perror("strdup failed");
            continue;
        }

        pthread_t tid;
        if (pthread_create(&tid, NULL, file_worker, filename_copy) != 0) {
            perror("pthread_create failed");
            free(filename_copy);
            continue;
        }

        pthread_detach(tid);

        pthread_mutex_lock(&count_mutex);
        request_count++;
        pthread_mutex_unlock(&count_mutex);
    }

    pthread_mutex_lock(&count_mutex);
    printf("[Server] Total file requests handled: %d\n", request_count);
    pthread_mutex_unlock(&count_mutex);

    printf("[Server] Goodbye!\n");
    return 0;
}

