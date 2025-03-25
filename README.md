## T-Tex

``git clone https://github.com/mittalswastik/qpu.git`` \
``cd qpu``\
``git clone https://github.com/mittalswastik/llvm-project.git`` \
``cd lvm-project`` \
``git checkout quantum_lib``

Repository should be: ~/qpu/llvm-project
  
### T-Tex Setup: Manual installation (if scr script runs into issues)

1. ``sudo apt update``
2. ``sudo apt install libgmp-dev libmpfr-dev``
3. ``sudo apt install texinfo``
4. ``sudo apt install bison``
5. ``sudo apt install m4``
6. ``sudo apt install pybind11-dev``
7. ``sudo apt install libffi-dev`` - This is to allow other offload devices than GPU (foreign function interface - run ``export LIBOMPTARGET_DEBUG=1`` before task compilation to identify offloading issues)
8. ``mkdir build``
9. ``git clone --depth 1 git://sourceware.org/git/binutils-gdb.git binutils``
10. ``cd build``
11. ``CC='gcc' CXX='g++' cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_USE_LINKER=gold  -DLLVM_BINUTILS_INCDIR=../binutils/include -DLLVM_ENABLE_PROJECTS="clang;lld;openmp" DLIBOMP_OMPT_OPTIONAL=ON DLIBIOMP_OMPT_SUPPORT=ON DLIBOMP_USE_DEBUGGER=ON -G "Ninja" ../llvm``
12. ``sudo ninja -j4`` \
   *``-jn``, n is number of threads used to build llvm (more n results in faster build but slower linking) \
   *g++ version issue:
        ** ``unintptr_t`` was not declared error: Fix - add [``#include <cstdint>``](https://github.com/mittalswastik/llvm-project/blob/82d8185c19fca4eb25f4e45ce1036d9c06ea7470/llvm/include/llvm/Support/Signals.h#L17)
13. ``../binutils/configure --enable-gold --enable-plugins --disable-werror``
14. ``make all-gold``
15. ``cd ..``
16. ``mkdir build_offload``
17. ``cd build_offload``
18. ``cmake -DCMAKE_CXX_COMPILER=~/qpu/llvm-project/build/bin/clang++  -DCMAKE_C_COMPILER=~/qpu/llvm-project/build/bin/clang  -DCUDAToolkit_INCLUDE_DIRECTORIES="/usr/local/cuda-11.4"  -DCMAKE_BUILD_TYPE=Release -DLLVM_USE_LINKER=gold -DLLVM_TARGETS_TO_BUILD="X86;NVPTX" -DLLVM_ENABLE_PROJECTS="clang;lld;openmp" -DLLVM_ENABLE_RUNTIMES="offload" -G "Unix Makefiles" ../llvm``
19. ``sudo make -j4``

### Execution flags

* Either copy omp.h & omp_tools.h (should be in ~/build/projects/openmp/runtime/src/) generated after compiling llvm to the same directory as the benchmark or add path using -I flag
  * omp.h - OpenMP runtime library
  * omp-tools.h - OpenMP ompt profiler too

### Execution

``cd ~/qpu`` \
``export LD_LIBRARY_PATH=/qpu/llvm-project/build_offload/lib:$LD_LIBRARY_PATH`` \
``~/qpu/llvm-project/build_offload/bin/clang++  -g test_omp_offload.cpp  -fopenmp -fopenmp-targets=x86_64-unknown-linux-gnu --offload-arch=x86-64 -o test`` \
``./test``

