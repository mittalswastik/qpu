#include <iostream>
#include <vector>
#include <omp.h>
#include <omp-tools.h>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <bits/stdc++.h>
#include <unistd.h>


#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdio>

#define N 10

using namespace std;

extern "C" ompt_start_tool_result_t* ompt_start_tool(
  unsigned int omp_version,
  const char *runtime_version)
{
  return nullptr;
}

// Function to execute a command and get the output
int main() {
    int n = omp_get_num_threads();
    // Check for the number of devices available
    int num_devices = omp_get_num_devices();
    printf("Number of available devices: %d\n", num_devices);
    
    // // If no device is available, exit
    if (num_devices < 1) {
        printf("No devices available for offloading. Exiting.\n");
        return 1;
    }

    // Arrays for computation
    int a[N], b[N], c[N];

    // Initialize arrays
    for (int i = 0; i < N; i++) {
        a[i] = i;
        b[i] = 2 * i;
        c[i] = 0;
    }

    // Offload computation to GPU (assuming device 0)
    #pragma omp target device(0) map(to: a[0:N], b[0:N]) map(from: c[0:N])
    {
        for (int i = 0; i < N; i++) {
            c[i] = a[i]+(b[i]/2);
        }
    }

    return 0;
}
