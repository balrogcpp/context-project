# which compilers to use for C and C++
# cross compilers to use for C, C++ and Fortran
string(APPEND CMAKE_EXE_LINKER_FLAGS " -fuse-ld=lld")
string(APPEND CMAKE_SHARED_LINKER_FLAGS " -fuse-ld=lld")

set(CMAKE_AR llvm-ar)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
