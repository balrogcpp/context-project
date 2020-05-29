FROM balrogcpp/mingw-gcc10-cmake:latest

ARG CONTEXT_HOME=/mnt/context-demo
WORKDIR ${CONTEXT_HOME}

ADD sources ./sources
ADD deploy ./deploy
ADD CMake ./CMake
ADD demo ./demo
ADD doc ./doc
ADD tests ./tests
ADD thirdparty/CMakeLists.txt ./thirdparty/CMakeLists.txt
ADD CMakeLists.txt .
ADD LICENSE .
ADD programs ./programs
ADD scenes ./scenes
ADD gui ./gui
ADD android ./android
ADD .git ./.git

RUN mkdir -p ./build-mingw && mkdir -p ./build-gnu && mkdir -p ./build-android

WORKDIR ${CONTEXT_HOME}/build-mingw
RUN cmake -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-mingw.cmake -G Ninja ..
RUN cmake --build . --target context-deps
RUN cmake -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-mingw.cmake -G Ninja .. && cmake --build . --target context-package
RUN cmake --build . --target install

WORKDIR ${CONTEXT_HOME}/build-gnu
RUN cmake -G Ninja ..
RUN cmake --build . --target context-pdf > /dev/null
RUN cmake --build . --target context-deps
RUN cmake -G Ninja .. && cmake --build . --target context-package
RUN cmake --build . --target install
RUN cmake --build . --target context-install-zip

#WORKDIR /home/user/context-demo/build-android
#RUN cmake -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK_HOME}/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=16 -G Ninja ..
#RUN cmake --build . --target context-deps
#RUN cmake -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK_HOME}/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=16 -G Ninja .. && cmake --build . --target context-demo