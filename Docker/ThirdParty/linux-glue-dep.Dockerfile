FROM registry.gitlab.com/balrogcpp/context-project/clang-linux

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

COPY ./ThirdParty/CMakeLists.txt ./ThirdParty/CMakeLists.txt
COPY ./ThirdParty/Patch ./ThirdParty/Patch
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./CMake ./CMake

RUN apt-get update \
     && apt-get install --no-install-recommends -y gcc libxaw7-dev libxrandr-dev libglew-dev libpulse-dev \
     && apt-get clean \
     && mkdir ${CONTEXT_HOME}/build-linux && cd ${CONTEXT_HOME}/build-linux \
     && export CC=clang \
     && export CXX=clang++ \
     && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-linux.cmake -G Ninja .. \
     && cmake --build . --target ThirdParty

RUN cd ${CONTEXT_HOME}/ThirdParty/External/Linux_x86_64_Clang_Release \
    && rm -rf src tmp \
    && cd  ${CONTEXT_HOME}/ThirdParty/External \
    && tar cfJ Linux_x86_64_Clang_Release.tar.xz Linux_x86_64_Clang_Release \
    && mv Linux_x86_64_Clang_Release.tar.xz ${CONTEXT_HOME}/Artifacts
