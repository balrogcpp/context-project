FROM balrogcpp/deps-cross:latest

ARG DEBIAN_FRONTEND=noninteractive
ARG CONTEXT_HOME=/mnt/build
ARG GIT_HASH=00000000
WORKDIR ${CONTEXT_HOME}

COPY ./sources ./sources
COPY ./deploy ./deploy
COPY ./cmake ./cmake
COPY ./sample ./sample
COPY ./doc ./doc
COPY ./tests ./tests
COPY ./LICENSE .
COPY ./programs ./programs
COPY ./assets ./assets
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./dependencies/CMakeLists.txt ./dependencies/CMakeLists.txt


RUN cmake -P cmake/flat_zip.cmake


RUN mkdir build-windows && cd build-windows \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-mingw.cmake -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && cmake --build . --target build-package \
    && cd .. \
    && rm -rf build-windows


RUN mkdir build-apple && cd build-apple \
    && eval `x86_64-apple-darwin19-osxcross-conf` \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-clang-apple.cmake -DGIT_SHA1=$GIT_HASH -G Ninja .. \
    && cmake --build . --target build-package \
    && cd .. \
    && rm -rf build-apple

COPY ./android ./android

RUN cd ./android \
    && ./gradlew assembleRelease \
    && cd ../ \
    && rm -rf ${CONTEXT_HOME}/dependencies/external \
    && rm -rf ${ANDROID_HOME} \
    && cd ../

RUN mv android/app/build/outputs/apk/release/app-arm64-v8a-release.apk artifacts/app-arm64-v8a-$GIT_HASH.apk
