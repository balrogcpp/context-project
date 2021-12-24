FROM registry.gitlab.com/balrogcpp/context-project/clang-windows

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

COPY ./ThirdParty/CMakeLists.txt ./ThirdParty/CMakeLists.txt
COPY ./ThirdParty/Patch ./ThirdParty/Patch
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./CMake ./CMake

RUN mkdir ${CONTEXT_HOME}/build-windows && cd ${CONTEXT_HOME}/build-windows \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-mingw.cmake -G Ninja .. \
    && cmake --build . --target ThirdParty

RUN cd ${CONTEXT_HOME}/ThirdParty/External/Windows_x86_64_Clang_Mingw_Release \
    && rm -rf src tmp \
    && cd ${CONTEXT_HOME}/ThirdParty/External \
    && tar cfJ Windows_x86_64_Clang_Mingw_Release.tar.xz Windows_x86_64_Clang_Mingw_Release \
    && mv Windows_x86_64_Clang_Mingw_Release.tar.xz ${CONTEXT_HOME}/Artifacts
