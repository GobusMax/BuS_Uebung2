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
    if (ftruncate(shared_memory_file_descriptor, sizeof(NUMBERS)) == -1) {
        switch (errno) {
            case 0:
                break;
            case EBADF:
                printf("fd is not a valid open file descriptor");
                return 1;
            case EINVAL:
                printf("The length argument was negative or larger than the maximum file size");
                return 1;
            case EFBIG:
                printf("The length argument was larger than the current file size");
                return 1;
            default:
                printf("Error truncating file");
                return 1;
        }
    }

    int(*shared_memory_value)[N_ELEMS] = mmap(NULL, sizeof(NUMBERS), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_file_descriptor, 0);
    if (shared_memory_value == MAP_FAILED) {
        switch (errno) {
            case 0:
                break;
            case EBADF:
                printf("fd is not a valid open file descriptor");
                return 1;
            case EINVAL:
                printf("The length argument was negative or larger than the maximum file size");
                return 1;
            case EFBIG:
                printf("The length argument was larger than the current file size");
                return 1;
            case ENOMEM:
                printf("There was insufficient room in the address space to map the file");
                return 1;
            case EACCES:
                printf("fd does not have permission to map the file");
                return 1;
            case EAGAIN:
                printf("The file has been locked, or too much memory has been locked");
                return 1;
            case ENODEV:
                printf("The underlying filesystem of the specified file does not support memory mapping");
                return 1;
            default:
                printf("Error mapping memory");
                return 1;
        }
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
        switch (errno) {
            case 0:
                break;
            case EBADF:
                printf("fd is not a valid open file descriptor");
                return 1;
            case EINVAL:
                printf("The length argument was negative or larger than the maximum file size");
                return 1;
            case EFBIG:
                printf("The length argument was larger than the current file size");
                return 1;
            case ENOMEM:
                printf("There was insufficient room in the address space to map the file");
                return 1;
            case EACCES:
                printf("fd does not have permission to map the file");
                return 1;
            case EAGAIN:
                printf("The file has been locked, or too much memory has been locked");
                return 1;
            case ENODEV:
                printf("The underlying filesystem of the specified file does not support memory mapping");
                return 1;
            default:
                printf("Error unmapping memory");
                return 1;
        }
    }
    if (close(shared_memory_file_descriptor) == -1) {
        switch (errno) {
            case 0:
                break;
            case EBADF:
                printf("fd is not a valid open file descriptor");
                return 1;
            case EINTR:
                printf("close() was interrupted by a signal");
                return 1;
            case EIO:
                printf("An I/O error occurred");
                return 1;
            default:
                printf("Error closing file");
                return 1;
        }
    }
    return 0;
}