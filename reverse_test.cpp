#include <iostream>
#include <omp.h>

#pragma omp requires reverse_offload

int main() {
    std::cout << "Starting on the host.\n";

    // Offload to GPU (device 0)
    //#pragma omp requires reverse_offload
    
    #pragma omp target device(0)
    {
        std::cout << "Running on the GPU!\n";
    }

    #pragma omp target device(ancestor:1)
    {
        std::cout << "Back on the host via reverse offload!\n";
    }

    return 0;
}