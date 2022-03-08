# This source file is part of Glue Engine. Created by Andrey Vasiliev

string(APPEND CMAKE_EXE_LINKER_FLAGS " -fuse-ld=lld")
string(APPEND CMAKE_SHARED_LINKER_FLAGS " -fuse-ld=lld")

set(CMAKE_AR llvm-ar)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
