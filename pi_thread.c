#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
unsigned long num_iterations_per_thread = 0;
unsigned long num_iterations = 0;
unsigned long num_threads = 1;
typedef struct thread_data {
    unsigned int idx;
    unsigned long res;
} thread_data;

void* calc_pi_helper(void* args) {
    thread_data* tdata = (thread_data*)args;
    // srand(tdata->idx);
    unsigned int seed = tdata->idx;
    unsigned long count = 0;

    for (unsigned long i = 0; i < num_iterations_per_thread; i++) {
        float x = (float)rand_r(&seed) / (float)(RAND_MAX);
        float y = (float)rand_r(&seed) / (float)(RAND_MAX);
        if (x * x + y * y <= 1.) {
            count++;
        }
    }
    tdata->res = count;
    return 0;
}

int main(int argc, char** argv) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0 && i < argc - 1) {
            num_iterations = atoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-t") == 0 && i < argc - 1) {
            num_threads = atoi(argv[i + 1]);
        }
    }
    num_iterations_per_thread = num_iterations / num_threads;
    pthread_t threads[num_threads];
    thread_data tdata[num_threads];
    for (unsigned long i = 0; i < num_threads; i++) {
        tdata[i].res = 0;
        tdata[i].idx = i;
        pthread_create(&threads[i], NULL, &calc_pi_helper, &tdata[i]);
    }
    for (unsigned long i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    unsigned long sum = 0;
    for (unsigned long i = 0; i < num_threads; i++) {
        sum += tdata[i].res;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    double delta = (end.tv_sec - start.tv_sec);
    delta += (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("RESULT: %lf \nTIME: %f", 4 * (double)sum / (double)num_iterations, delta);
    return 0;
}
