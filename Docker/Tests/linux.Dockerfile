FROM registry.gitlab.com/balrogcpp/context-project/clang-linux

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

RUN apt-get update \
    && apt-get install --no-install-recommends -y libxaw7-dev libxrandr-dev libglew-dev libpulse-dev libgles2-mesa-dev libegl1-mesa-dev \
    && apt-get clean

COPY ./Source ./Source
COPY ./Deploy ./Deploy
COPY ./CMake ./CMake
COPY ./LICENSE .
COPY ./Programs ./Programs
COPY ./Assets ./Assets
COPY ./Binaries ./Binaries
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./ThirdParty/CMakeLists.txt ./ThirdParty/CMakeLists.txt

RUN mkdir -p ${CONTEXT_HOME}/ThirdParty/External \
    && tar Jxfp ${CONTEXT_HOME}/Binaries/Dependencies/Linux_x86_64_Clang_Release.tar.xz -C ${CONTEXT_HOME}/ThirdParty/External

RUN mkdir build-linux && cd build-linux \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-linux.cmake -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && cmake --build . --target BuildPackage \
    && rm -rf ../build-linux
