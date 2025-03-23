#include <stdio.h>
#include <omp.h>

int main() {
    #ifdef _OPENMP
        printf("OpenMP version: %d\n", _OPENMP);
    #else
        printf("OpenMP is not supported.\n");
    #endif
    return 0;
}
