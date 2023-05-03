#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
unsigned long num_iterations_per_thread = 0;
unsigned long num_iterations = 0;
unsigned long num_threads = 1;
typedef struct thread_data {
    unsigned int idx;
    unsigned long res;
} thread_data;

void* calc_pi_helper(void* args) {
    thread_data* tdata = (thread_data*)args;
    // initialen seed für jeden thread anders
    unsigned int seed = tdata->idx;
    unsigned long count = 0;

    for (unsigned long i = 0; i < num_iterations_per_thread; i++) {
        // rand_r ist thread safe
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
    if (clock_gettime(CLOCK_MONOTONIC, &start)) {
        switch (errno) {
            case 0:
                break;
            case EFAULT:
                printf("tp points outside the accessible address space");
                return 1;
            case EINVAL:
                printf("The clk_id specified is not supported on this system");
                return 1;
            case EPERM:
                printf("clock_settime() does not have permission to set the clock indicated");
                return 1;
            default:
                printf("Error getting time");
                return 1;
        }
    }

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
        if (pthread_create(&threads[i], NULL, &calc_pi_helper, &tdata[i])) {
            switch (errno) {
                case 0:
                    break;
                case EAGAIN:
                    printf("Insufficient resources to create another thread");
                    return 1;
                case EINVAL:
                    printf("Invalid settings in attr");
                    return 1;
                case EPERM:
                    printf("No permission to set the scheduling policy and parameters specified in attr");
                    return 1;
                default:
                    printf("Error creating thread");
                    return 1;
            }
        }
    }

    for (unsigned long i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], NULL)) {
            switch (errno) {
                case 0:
                    break;
                case EDEADLK:
                    printf("A deadlock was detected (e.g., two threads tried to join with each other); or thread specifies the calling thread");
                    return 1;
                case EINVAL:
                    printf("thread is not a joinable thread");
                    return 1;
                case ESRCH:
                    printf("No thread with the ID thread could be found");
                    return 1;
                default:
                    printf("Error joining thread");
                    return 1;
            }
        }
    }
    unsigned long sum = 0;
    for (unsigned long i = 0; i < num_threads; i++) {
        sum += tdata[i].res;
    }
    if (clock_gettime(CLOCK_MONOTONIC, &end)) {
        switch (errno) {
            case 0:
                break;
            case EFAULT:
                printf("tp points outside the accessible address space");
                return 1;
            case EINVAL:
                printf("The clk_id specified is not supported on this system");
                return 1;
            case EPERM:
                printf("clock_settime() does not have permission to set the clock indicated");
                return 1;
            default:
                printf("Error getting time");
                return 1;
        }
    }

    double delta = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("RESULT: %lf \nTIME: %f", 4 * (double)sum / (double)num_iterations, delta);
    return 0;
}
