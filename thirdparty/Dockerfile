#MIT License
#
#Copyright (c) 2020 Andrey Vasiliev
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in all
#copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#SOFTWARE.

FROM balrogcpp/gcc-mingw-x86_64:latest

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/context-demo
WORKDIR ${CONTEXT_HOME}

ADD ./thirdparty/CMakeLists.txt ./thirdparty/CMakeLists.txt
ADD CMakeLists.txt .
ADD ./CMake ./CMake

RUN apt-get install -y git ninja-build libxaw7-dev libxrandr-dev libglew-dev libpulse-dev > /dev/null \
    && mkdir -p ./build-windows && mkdir -p ./build-linux && mkdir -p ./build-android \
    && cd ${CONTEXT_HOME}/build-windows \
    && cmake -DCONTEXT_ONLY_DEPS=true -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-mingw.cmake -G Ninja .. \
    && cmake --build . --target context-deps \
    && cd ${CONTEXT_HOME}/thirdparty/external/windows-mingw/Release/ \
    && rm -rf src \
    && cd ${CONTEXT_HOME}/build-linux \
    && cmake -DCONTEXT_ONLY_DEPS=true -G Ninja .. \
    && cmake --build . --target context-deps \
    && cd ${CONTEXT_HOME}/thirdparty/external/linux-gcc/Release/ \
    && rm -rf src