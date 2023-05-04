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
    int shared_memory_file_descriptor = shm_open(MEM_NAME, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    if (shared_memory_file_descriptor == -1) {
        perror("shm_open");
    }
    if (ftruncate(shared_memory_file_descriptor, sizeof(NUMBERS)) == -1) {
        perror("ftuncate");
    }

    int(*shared_memory_value)[N_ELEMS] = mmap(NULL, sizeof(NUMBERS), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_file_descriptor, 0);
    if (shared_memory_value == MAP_FAILED) {
        perror("mmap");
    }

    for (int i = 0; i < N_ELEMS; i++) {
        *shared_memory_value[i] = NUMBERS[i];
    }
    int counter = 0;
    int sorted = 0;
    while (!sorted) {
        int sorted_temp = 1;
        for (int i = 0; i < N_ELEMS; i++) {
            printf("%d", *shared_memory_value[i]);
            if (i >= 1 && *shared_memory_value[i - 1] > *shared_memory_value[i]) {
                sorted_temp = 0;
            }
        }
        sorted = sorted_temp;
        printf("\n");
        sleep(1);
    }
    if (munmap(shared_memory_value, sizeof(shared_memory_value)) == -1) {
        perror("munmap");
    }
    if (close(shared_memory_file_descriptor) == -1) {
        perror("close");
    }
    return 0;
}