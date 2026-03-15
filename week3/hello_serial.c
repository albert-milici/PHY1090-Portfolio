#include <stdio.h>
#include <stdlib.h>

// simple script to try and replicate what hello_mpi.c does as a serial task

int main(int argc, char **argv) {

    int n = atoi(argv[1]);
    
    for (int i = 0; i < n; i++) {
        printf("Hello, I am %d of %d\n", i, n);
    }
    return 0;
}