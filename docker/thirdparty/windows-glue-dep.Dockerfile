FROM balrogcpp/clang-windows:latest
ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

COPY ./thirdparty/CMakeLists.txt ./thirdparty/CMakeLists.txt
COPY ./thirdparty/patch ./thirdparty/patch
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./doc ./doc
COPY ./cmake ./cmake

RUN mkdir ${CONTEXT_HOME}/build-windows && cd ${CONTEXT_HOME}/build-windows \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-mingw.cmake -G Ninja .. \
    && cmake --build . --target thirdparty \
    && cd ${CONTEXT_HOME}/thirdparty/external/Release/windows-clang-mingw-x86_64 \
    && rm -rf src tmp ${CONTEXT_HOME}/build-windows

RUN cd  ${CONTEXT_HOME}/thirdparty/external/Release \
    && tar cfJ windows-clang-mingw-x86_64.tar.xz windows-clang-mingw-x86_64 \
    && mv windows-clang-mingw-x86_64.tar.xz ${CONTEXT_HOME}/artifacts
