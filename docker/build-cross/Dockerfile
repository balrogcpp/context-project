FROM balrogcpp/deps-cross:latest

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}


ADD ./sources ./sources
ADD ./deploy ./deploy
ADD ./cmake ./cmake
ADD ./demo ./demo
ADD ./doc ./doc
ADD ./tests ./tests
ADD ./LICENSE .
ADD ./programs ./programs
ADD ./assets ./assets
ADD ./CMakeLists.txt .
ADD ./zip-dependencies.py .
ADD ./dependencies/CMakeLists.txt ./dependencies/CMakeLists.txt


RUN python3 zip-dependencies.py


RUN mkdir build-windows && cd build-windows \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-mingw.cmake -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && cmake --build . --target package \
    && cd .. \
    && rm -rf build-windows


RUN mkdir build-apple && cd build-apple \
    && eval `x86_64-apple-darwin19-osxcross-conf` \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-apple.cmake -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && cmake --build . --target package \
    && cd .. \
    && rm -rf build-apple


ADD ./android ./android

RUN cd ./android \
    && ./gradlew assembleRelease \
    && cd ../ \
    && rm -rf ${CONTEXT_HOME}/dependencies/external \
    && rm -rf ${ANDROID_HOME} \
    && cd ../

RUN mv android/app/build/outputs/apk/release/app-arm64-v8a-release.apk artifacts/app-arm64-v8a-$GIT_HASH.apk
