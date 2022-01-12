FROM registry.gitlab.com/balrogcpp/context-project/clang-unicross

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

RUN apt-get update \
    && apt-get install --no-install-recommends -y libxaw7-dev libxrandr-dev libglew-dev libpulse-dev libgles2-mesa-dev libegl1-mesa-dev \
    && apt-get clean

COPY ./Source ./Source
COPY ./ThirdParty/CMakeLists.txt ./ThirdParty/CMakeLists.txt
COPY ./ThirdParty/Patch ./ThirdParty/Patch
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./CMake ./CMake

RUN  mkdir ${CONTEXT_HOME}/build-linux && cd ${CONTEXT_HOME}/build-linux \
     && export CC=clang \
     && export CXX=clang++ \
     && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-linux.cmake -G Ninja .. \
     && cmake --build . --target ThirdParty

RUN cd ${CONTEXT_HOME}/ThirdParty/External/Linux_x86_64_Clang_Release \
    && rm -rf src tmp \
    && cd  ${CONTEXT_HOME}/ThirdParty/External \
    && tar cfJ Linux_x86_64_Clang_Release.tar.xz Linux_x86_64_Clang_Release \
    && mv Linux_x86_64_Clang_Release.tar.xz ${CONTEXT_HOME}/Artifacts

RUN mkdir ${CONTEXT_HOME}/build-windows && cd ${CONTEXT_HOME}/build-windows \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-mingw.cmake -G Ninja .. \
    && cmake --build . --target ThirdParty

RUN cd ${CONTEXT_HOME}/ThirdParty/External/Windows_x86_64_Clang_Mingw_Release \
    && rm -rf src tmp \
    && cd ${CONTEXT_HOME}/ThirdParty/External \
    && tar cfJ Windows_x86_64_Clang_Mingw_Release.tar.xz Windows_x86_64_Clang_Mingw_Release \
    && mv Windows_x86_64_Clang_Mingw_Release.tar.xz ${CONTEXT_HOME}/Artifacts

RUN mkdir ${CONTEXT_HOME}/build-apple && cd ${CONTEXT_HOME}/build-apple \
    && eval `x86_64-apple-darwin20.4-osxcross-conf` \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-apple.cmake -G Ninja .. \
    && cmake --build . --target ThirdParty

RUN cd ${CONTEXT_HOME}/ThirdParty/External/Darwin_x86_64_Clang_Release \
    && rm -rf src tmp \
    && cd  ${CONTEXT_HOME}/ThirdParty/External \
    && tar cfJ Darwin_x86_64_Clang_Release.tar.xz Darwin_x86_64_Clang_Release \
    && mv Darwin_x86_64_Clang_Release.tar.xz ${CONTEXT_HOME}/Artifacts

