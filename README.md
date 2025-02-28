## T-Tex

Clone this repository
Clone the repository and switch to branch [ttex_update](https://github.com/mittalswastik/ttex_implementation/tree/ttex_update)
  
### T-Tex Setup: Manual installation (if scr script runs into issues)

1. ``sudo apt update``
2. ``sudo apt install libgmp-dev libmpfr-dev``
3. ``sudo apt install texinfo``
4. ``sudo apt install bison``
5. ``sudo apt install m4``
6. ``git clone https://github.com/mittalswastik/ttex_implementation.git``
7. ``cd ttex_implementation/``
8. ``mkdir build``
9. ``git clone --depth 1 git://sourceware.org/git/binutils-gdb.git binutils``
10. ``cd build``
11. ``CC='gcc' CXX='g++' cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_USE_LINKER=gold  -DLLVM_BINUTILS_INCDIR=../binutils/include -DLLVM_ENABLE_PROJECTS="clang;lld;openmp" DLIBOMP_OMPT_OPTIONAL=ON DLIBIOMP_OMPT_SUPPORT=ON DLIBOMP_USE_DEBUGGER=ON -G "Ninja" ../llvm``
12. ``sudo ninja -j4`` \
   *``-jn``, n is number of threads used to build llvm (more n results in faster build but slower linking) \
   *g++ version issue:
        ** ``unintptr_t`` was not declared error: Fix - add [``#include <cstdint>``](https://github.com/mittalswastik/ttex_implementation/blob/e7740356386e840ddadd91d502daf8a8ef612f21/llvm/include/llvm/Support/Signals.h#L18)
13. ``../binutils/configure --enable-gold --enable-plugins --disable-werror``
14. ``make all-gold``
