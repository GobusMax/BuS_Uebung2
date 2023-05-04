#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
int main(void) {
    int shared_memory_file_descriptor = shm_open(MEM_NAME, O_RDWR, 0);
    if (shared_memory_file_descriptor == -1) {
        perror("shm_open");
    }

    int(*shared_memory_value)[N_ELEMS] = mmap(NULL, sizeof(NUMBERS), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_file_descriptor, 0);
    if (shared_memory_value == MAP_FAILED) {
        perror("mmap");
    }
    for (int i = 1; i < N_ELEMS; i++) {
        if (*shared_memory_value[i - 1] > *shared_memory_value[i]) {
            int temp = *shared_memory_value[i];
            *shared_memory_value[i] = *shared_memory_value[i - 1];
            *shared_memory_value[i - 1] = temp;
        }
    }
    if (munmap(shared_memory_value, sizeof(shared_memory_value)) == -1) {
        perror("munmap");
    }
    if (close(shared_memory_file_descriptor) == -1) {
        perror("close");
    }
    return 0;
}