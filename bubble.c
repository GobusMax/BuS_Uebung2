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
        switch (errno) {
            case 0:
                break;
            case EACCES:
                printf("The shared memory object exists and the permissions specified by oflag are denied, or the shared memory object does not exist and permission to create the shared memory object is denied");
                return 1;
            case EEXIST:
                printf("Both O_CREAT and O_EXCL were specified and the shared memory object specified by name already exists");
                return 1;
            case EINVAL:
                printf("name contained a slash");
                return 1;
            case EMFILE:
                printf("The process already has the maximum number of files open");
                return 1;
            case ENAMETOOLONG:
                printf("name was too long");
                return 1;
            case ENFILE:
                printf("The limit on the total number of files open on the system has been reached");
                return 1;
            case ENOENT:
                printf("O_CREAT is not set and the named shared memory object does not exist");
                return 1;
            case ENOMEM:
                printf("Insufficient memory");
                return 1;
            case EROFS:
                printf("name refers to a file on a read-only file system");
                return 1;
            default:
                printf("Error creating file");
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
    for (int i = 1; i < N_ELEMS; i++) {
        if (*shared_memory_value[i - 1] > *shared_memory_value[i]) {
            int temp = *shared_memory_value[i];
            *shared_memory_value[i] = *shared_memory_value[i - 1];
            *shared_memory_value[i - 1] = temp;
        }
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