FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y wget zip openjdk-8-jdk

RUN update-alternatives --set java /usr/lib/jvm/java-8-openjdk-amd64/jre/bin/java

ARG GRADLE_VERSION=6.4.1
ARG GRADLE_HOME=/opt/gradle
RUN cd /tmp && \
    wget -q https://services.gradle.org/distributions/gradle-${GRADLE_VERSION}-bin.zip && \
    unzip -q gradle-${GRADLE_VERSION}-bin.zip && \
    mv ./gradle-${GRADLE_VERSION}/ ${GRADLE_HOME} && \
    rm gradle-${GRADLE_VERSION}-bin.zip
ENV PATH="${GRADLE_HOME}/bin:${PATH}"

ARG ANDROID_SDK_VERSION=r30.0.1
ARG ANDROID_NDK_VERSION=r21b
ARG ANDROID_SDK_HOME=/opt/android-sdk
RUN mkdir ${ANDROID_SDK_HOME} && \
    cd /tmp && \
	wget --output-document=platform-tools.zip --quiet https://dl.google.com/android/repository/platform-tools_r30.0.1-linux.zip && \
        unzip platform-tools.zip && \
        mv platform-tools ${ANDROID_SDK_HOME} && \
        rm platform-tools.zip &&\
	wget --output-document=tools.zip --quiet https://dl.google.com/android/repository/sdk-tools-linux-4333796.zip && \
        unzip tools.zip && \
        mv tools ${ANDROID_SDK_HOME} && \
        rm tools.zip &&\
	wget --output-document=platform.zip --quiet https://dl.google.com/android/repository/platform-29_r04.zip && \
        unzip platform.zip && \
        mv android-10 android-29 && \
        mkdir ${ANDROID_SDK_HOME}/platforms && \
        mv android-29 ${ANDROID_SDK_HOME}/platforms && \
        rm platform.zip &&\
	wget --output-document=build-tools.zip --quiet https://dl.google.com/android/repository/build-tools_r29.0.3-linux.zip && \
        unzip build-tools.zip && \
        mv android-10 29.0.3 && \
        mkdir ${ANDROID_SDK_HOME}/build-tools && \
        mv 29.0.3 ${ANDROID_SDK_HOME}/build-tools && \
        rm build-tools.zip && \
    wget -q https://dl.google.com/android/repository/android-ndk-${ANDROID_NDK_VERSION}-linux-x86_64.zip && \
        unzip -q android-ndk-${ANDROID_NDK_VERSION}-linux-x86_64.zip && \
        mv android-ndk-${ANDROID_NDK_VERSION} ${ANDROID_SDK_HOME}/ndk && \
        rm android-ndk-${ANDROID_NDK_VERSION}-linux-x86_64.zip
RUN cd ${ANDROID_SDK_HOME}/tools/bin && yes | ./sdkmanager --licenses
ENV ANDROID_SDK_ROOT="${ANDROID_SDK_HOME}"
ENV PATH="${ANDROID_SDK_HOME}/ndk:${PATH}"