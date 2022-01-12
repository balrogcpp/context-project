FROM registry.gitlab.com/balrogcpp/context-project/clang-unicross

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

RUN apt-get update \
    && apt-get install --no-install-recommends -y libxaw7-dev libxrandr-dev libglew-dev libpulse-dev libgles2-mesa-dev libegl1-mesa-dev libdbus-1-dev \
    && apt-get clean

COPY ./Binaries ./Binaries

RUN mkdir -p ${CONTEXT_HOME}/ThirdParty/External \
    && tar Jxfp ${CONTEXT_HOME}/Binaries/Dependencies/Linux_x86_64_Clang_Release.tar.xz -C ${CONTEXT_HOME}/ThirdParty/External

RUN mkdir -p ${CONTEXT_HOME}/ThirdParty/External \
    && tar Jxfp ${CONTEXT_HOME}/Binaries/Dependencies/Windows_x86_64_Clang_Mingw_Release.tar.xz -C ${CONTEXT_HOME}/ThirdParty/External

RUN mkdir -p ${CONTEXT_HOME}/ThirdParty/External \
    && tar Jxfp ${CONTEXT_HOME}/Binaries/Dependencies/Darwin_x86_64_Clang_Release.tar.xz -C ${CONTEXT_HOME}/ThirdParty/External

COPY ./Source ./Source
COPY ./Deploy ./Deploy
COPY ./CMake ./CMake
COPY ./LICENSE .
COPY ./Programs ./Programs
COPY ./Assets ./Assets
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./ThirdParty/CMakeLists.txt ./ThirdParty/CMakeLists.txt

RUN mkdir build-linux && cd build-linux \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-linux.cmake -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && cmake --build . --target BuildPackage \
    && rm -rf ../build-linux

RUN mkdir build-windows && cd build-windows \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-mingw.cmake -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && cmake --build . --target BuildPackage \
    && rm -rf ../build-windows

RUN mkdir build-apple && cd build-apple \
    && eval `x86_64-apple-darwin20.4-osxcross-conf` \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-apple.cmake -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && cmake --build . --target BuildPackage \
    && rm -rf ../build-apple
