FROM balrogcpp/xio-deps-android:latest

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
ADD ./android ./android

RUN apt-get update \
    && apt-get install -y python3 \
    && apt-get clean


RUN python3 zip-dependencies.py


RUN cd ./android \
    && yes | sdkmanager  --licenses --sdk_root=${ANDROID_HOME} \
    && ./gradlew assembleRelease \
    && cd ../ \
    && mkdir artifacts \
    && mv ./android/app/build/outputs/apk/release/app-arm64-v8a-release.apk ./artifacts \
    && mv ./android/app/build/outputs/apk/release/app-x86_64-release.apk ./artifacts
