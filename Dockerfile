FROM balrogcpp/context-project-dependencies:latest

ARG CONTEXT_HOME=/mnt/context-demo
WORKDIR ${CONTEXT_HOME}

ADD sources ./sources
ADD deploy ./deploy
ADD CMake ./CMake
ADD demo ./demo
ADD doc ./doc
ADD tests ./tests
ADD LICENSE .
ADD cegui ./cegui
ADD android ./android
ADD programs ./programs
ADD scenes ./scenes
ADD CMakeLists.txt .
ADD thirdparty/CMakeLists.txt ./thirdparty/CMakeLists.txt
ADD .git ./.git

RUN mkdir -p ./build-windows && mkdir -p ./build-linux && mkdir -p ./build-android

WORKDIR ${CONTEXT_HOME}/build-windows
RUN cmake -DCONTEXT_ONLY_DEPS=false -DCMAKE_TOOLCHAIN_FILE=../CMake/toolchain-mingw.cmake -G Ninja .. && cmake --build . --target install

WORKDIR ${CONTEXT_HOME}/build-linux
RUN cmake -DCONTEXT_ONLY_DEPS=false -G Ninja .. && cmake --build . --target install
# RUN cmake --build . --target context-pdf > /dev/null

RUN cmake --build . --target context-install-zip

#WORKDIR /home/user/context-demo/build-android
#RUN cmake -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK_HOME}/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=16 -G Ninja ..
#RUN cmake --build . --target context-deps
#RUN cmake -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK_HOME}/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=16 -G Ninja .. && cmake --build . --target context-demo