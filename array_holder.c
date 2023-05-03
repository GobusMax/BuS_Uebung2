#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int shared_memory_file_descriptor = shm_open(MEM_NAME, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    ftruncate(shared_memory_file_descriptor, sizeof(NUMBERS));
    int(*shared_memory_value)[N_ELEMS] = mmap(NULL, sizeof(NUMBERS), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_file_descriptor, 0);
    // shared_memory_value = NUMBERS;
    for (int i = 0; i < N_ELEMS; i++) {
        *shared_memory_value[i] = NUMBERS[i];
    }
    int counter = 0;
    int sorted = 0;
    while (!sorted) {
        counter++;
        if (counter % 10000000 == 0) {
            int sorted_temp = 1;
            for (int i = 0; i < N_ELEMS; i++) {
                printf("%d", *shared_memory_value[i]);
                if (i >= 1 && *shared_memory_value[i - 1] > *shared_memory_value[i]) {
                    sorted_temp = 0;
                }
            }
            sorted = sorted_temp;
            printf("\n");
        }
    }
    munmap(shared_memory_value, sizeof(shared_memory_value));
    return 0;
}