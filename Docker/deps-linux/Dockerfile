#MIT License
#
#Copyright (c) 2021 Andrew Vasiliev
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

FROM balrogcpp/clang-linux:latest

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
WORKDIR ${CONTEXT_HOME}

ADD ./dependencies/CMakeLists.txt ./dependencies/CMakeLists.txt
ADD ./dependencies/patch ./dependencies/patch
ADD ./CMakeLists.txt ./CMakeLists.txt
ADD ./doc ./doc
ADD ./CMake ./CMake

ENV CC=clang
ENV CXX=clang++


RUN apt-get update \
    && apt-get install -y git libxaw7-dev libxrandr-dev libglew-dev libpulse-dev libxml2


 RUN mkdir ${CONTEXT_HOME}/build-linux && cd ${CONTEXT_HOME}/build-linux \
     && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-clang-linux.cmake -G Ninja .. \
     && cmake --build . --target target-deps \
     && cd ${CONTEXT_HOME}/dependencies/external/Release/linux-clang-x86_64 \
     && rm -rf src tmp \
     && rm -rf ${CONTEXT_HOME}/build-linux


RUN cd ${CONTEXT_HOME} \
    && rm -rf doc CMake CMakeLists.txt dependencies/CMakeLists.txt dependencies/patch
