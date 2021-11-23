FROM balrogcpp/clang-apple:latest
ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

COPY ./dependencies/CMakeLists.txt ./dependencies/CMakeLists.txt
COPY ./dependencies/patch ./dependencies/patch
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./doc ./doc
COPY ./cmake ./cmake

RUN mkdir ${CONTEXT_HOME}/build-apple && cd ${CONTEXT_HOME}/build-apple \
    && eval `x86_64-apple-darwin19-osxcross-conf` \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-apple.cmake -G Ninja .. \
    && cmake --build . --target dependencies \
    && cd ${CONTEXT_HOME}/dependencies/external/Release/apple-darwin-clang-x86_64 \
    && rm -rf src tmp ${CONTEXT_HOME}/build-apple \
    && rm -rf ${CONTEXT_HOME}/build-apple

RUN cd  ${CONTEXT_HOME}/dependencies/external/Release \
    && tar cfJ apple-darwin-clang-x86_64.tar.xz apple-darwin-clang-x86_64 \
    && mv apple-darwin-clang-x86_64.tar.xz ${CONTEXT_HOME}/artifacts
