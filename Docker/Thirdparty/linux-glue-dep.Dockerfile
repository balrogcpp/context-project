FROM registry.gitlab.com/balrogcpp/context-project/clang-linux

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

COPY ./Thirdparty/CMakeLists.txt ./Thirdparty/CMakeLists.txt
COPY ./Thirdparty/Patch ./Thirdparty/Patch
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./CMake ./CMake

RUN apt-get update \
     && apt-get install --no-install-recommends -y gcc libxaw7-dev libxrandr-dev libglew-dev libpulse-dev \
     && apt-get clean \
     && mkdir ${CONTEXT_HOME}/build-linux && cd ${CONTEXT_HOME}/build-linux \
     && export CC=clang \
     && CXX=clang++ \
     && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-linux.cmake -G Ninja .. \
     && cmake --build . --target Thirdparty \
     && cd ${CONTEXT_HOME}/Thirdparty/External/Release/linux-clang-x86_64 \
     && rm -rf src tmp \
     && cd ${CONTEXT_HOME} \
     && rm -rf build-linux cmake CMakeLists.txt Thirdparty/CMakeLists.txt Thirdparty/patch

RUN cd  ${CONTEXT_HOME}/Thirdparty/External/Release \
    && tar cfJ linux-clang-x86_64.tar.xz linux-clang-x86_64 \
    && mv linux-clang-x86_64.tar.xz ${CONTEXT_HOME}/Artifacts
