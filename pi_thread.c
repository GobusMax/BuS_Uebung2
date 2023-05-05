
// Run with for example: ./pi_thread -n 1000000000 -t 4

#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
unsigned long num_iterations_per_thread = 0;
unsigned long num_iterations = 1000000000;
unsigned long num_threads = 1;
typedef struct thread_data {
    unsigned int idx;
    unsigned long res;
    unsigned long count;
} thread_data;

void* calc_pi_helper(void* args) {
    thread_data* tdata = (thread_data*)args;
    unsigned int seed = tdata->idx;
    for (unsigned long i = 0; i < num_iterations_per_thread; i++) {
        float x = (float)rand_r(&seed) / (float)(RAND_MAX);
        float y = (float)rand_r(&seed) / (float)(RAND_MAX);
        if (x * x + y * y <= 1.) {
            tdata->res++;
        }
        tdata->count++;
    }
    return 0;
}

int main(int argc, char** argv) {
    struct timespec start, end;
    if (clock_gettime(CLOCK_MONOTONIC, &start)) {
        perror("clock_gettime");
    }
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0 && i < argc - 1) {
            num_iterations = atol(argv[i + 1]);
        }
        if (strcmp(argv[i], "-t") == 0 && i < argc - 1) {
            num_threads = atol(argv[i + 1]);
        }
    }
    num_iterations_per_thread = num_iterations / num_threads;
    pthread_t threads[num_threads];
    thread_data tdata[num_threads];
    for (unsigned long i = 0; i < num_threads; i++) {
        tdata[i].res = 0;
        tdata[i].count = 0;
        tdata[i].idx = i;
        if (pthread_create(&threads[i], NULL, &calc_pi_helper, &tdata[i])) {
            perror("pthread_create");
        }
    }
    unsigned long count = 0;
    unsigned long sum = 0;

    struct timespec last, cur;
    if (clock_gettime(CLOCK_MONOTONIC, &last)) {
        perror("clock_gettime");
    }
    while (count < num_iterations) {
        count = 0;
        sum = 0;
        for (unsigned i = 0; i < num_threads; i++) {
            count += tdata[i].count;
            sum += tdata[i].res;
        }
        if (clock_gettime(CLOCK_MONOTONIC, &cur)) {
            perror("clock_gettime");
        }
        if ((cur.tv_sec + cur.tv_nsec * 1e-9) - (last.tv_sec + last.tv_nsec * 1e-9) >= 1) {
            printf("RESULT: %.16Lf \nITERATIONS: %-16ld  (%05.2f%%) \n\n", 4 * (long double)sum / (long double)count, count, (double)count / (double)num_iterations * 100.);
            if (clock_gettime(CLOCK_MONOTONIC, &last)) {
                perror("clock_gettime");
            }
        }
    }
    for (unsigned long i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], NULL)) {
            perror("pthread_join");
        }
    }
    if (clock_gettime(CLOCK_MONOTONIC, &end)) {
        perror("clock_gettime");
    }
    printf("FINAL\n");
    printf("RESULT: %.32lf \nITERATIONS: %ld \n", 4 * (double)sum / (double)count, count);
    printf("TIME: %lf\n", (end.tv_sec + end.tv_nsec * 1e-9) - (start.tv_sec + start.tv_nsec * 1e-9));
    return 0;
}
