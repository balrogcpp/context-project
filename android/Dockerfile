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

FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive
ARG GRADLE_VERSION=6.4.1
ARG GRADLE_HOME=/opt/gradle
ARG ANDROID_SDK_VERSION=r30.0.1
ARG ANDROID_NDK_VERSION=r21b
ARG ANDROID_SDK_HOME=/opt/android-sdk
ARG CMAKE_VERSION=3.18.1
ARG CMAKE_HOME=/opt/cmake

RUN apt-get update && \
    apt-get install -y wget zip openjdk-8-jdk && \
    update-alternatives --set java /usr/lib/jvm/java-8-openjdk-amd64/jre/bin/java && \
    cd /tmp && \
    wget https://services.gradle.org/distributions/gradle-${GRADLE_VERSION}-bin.zip && \
    unzip -q gradle-${GRADLE_VERSION}-bin.zip && \
    mv ./gradle-${GRADLE_VERSION}/ ${GRADLE_HOME} && \
    rm gradle-${GRADLE_VERSION}-bin.zip && \
    mkdir ${ANDROID_SDK_HOME} && \
    cd /tmp && \
	wget --output-document=platform-tools.zip https://dl.google.com/android/repository/platform-tools_r30.0.1-linux.zip && \
        unzip platform-tools.zip && \
        mv platform-tools ${ANDROID_SDK_HOME} && \
        rm platform-tools.zip &&\
	wget --output-document=tools.zip https://dl.google.com/android/repository/sdk-tools-linux-4333796.zip && \
        unzip tools.zip && \
        mv tools ${ANDROID_SDK_HOME} && \
        rm tools.zip &&\
	wget --output-document=platform.zip https://dl.google.com/android/repository/platform-29_r04.zip && \
        unzip platform.zip && \
        mv android-10 android-29 && \
        mkdir ${ANDROID_SDK_HOME}/platforms && \
        mv android-29 ${ANDROID_SDK_HOME}/platforms && \
        rm platform.zip &&\
	wget --output-document=build-tools.zip https://dl.google.com/android/repository/build-tools_r29.0.3-linux.zip && \
        unzip build-tools.zip && \
        mv android-10 29.0.3 && \
        mkdir ${ANDROID_SDK_HOME}/build-tools && \
        mv 29.0.3 ${ANDROID_SDK_HOME}/build-tools && \
        rm build-tools.zip && \
    wget https://dl.google.com/android/repository/android-ndk-${ANDROID_NDK_VERSION}-linux-x86_64.zip && \
        unzip -q android-ndk-${ANDROID_NDK_VERSION}-linux-x86_64.zip && \
        mv android-ndk-${ANDROID_NDK_VERSION} ${ANDROID_SDK_HOME}/ndk && \
        rm android-ndk-${ANDROID_NDK_VERSION}-linux-x86_64.zip && \
    cd ${ANDROID_SDK_HOME}/tools/bin && yes | ./sdkmanager --licenses && \
    cd /tmp && \
    wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh \
               -O /tmp/cmake-install.sh && \
        chmod u+x /tmp/cmake-install.sh && \
        mkdir /opt/cmake && \
        /tmp/cmake-install.sh --skip-license --prefix=${CMAKE_HOME} && \
        rm /tmp/cmake-install.sh

ENV PATH="${GRADLE_HOME}/bin:${PATH}"
ENV ANDROID_SDK_ROOT="${ANDROID_SDK_HOME}"
ENV PATH="${ANDROID_SDK_HOME}/ndk:${PATH}"
ENV PATH="${CMAKE_HOME}/bin:${PATH}"
