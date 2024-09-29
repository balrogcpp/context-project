FROM balrogcpp/gcc_oldlinux:latest


ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/var/build
ARG DEPS_DIR=${CONTEXT_HOME}/external
WORKDIR ${CONTEXT_HOME}


COPY ./source ./source
COPY ./external ./external
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./cmake ./cmake
COPY ./.git ./.git


# linux x86_64
RUN apt-get update \
    && apt-get -y install --no-install-recommends libxaw7-dev libxrandr-dev libglew-dev libpulse-dev libgles2-mesa-dev libegl1-mesa-dev libdbus-1-dev \
    && apt-get clean \
    && mkdir build && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-gcc-linux.cmake -G Ninja .. \
    && cmake --build . --target Contrib \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-gcc-linux.cmake -G Ninja .. \
    && cmake --build . --target package \
    && rm -rf ../artifacts/_CPack_Packages ../external/build ../external/sdk ../build \
    && apt-get -y purge libxaw7-dev libxrandr-dev libglew-dev libpulse-dev libgles2-mesa-dev libegl1-mesa-dev libdbus-1-dev \
    && apt-get -y autoremove
