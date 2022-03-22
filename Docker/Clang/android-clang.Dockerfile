# This file is part of Glue Engine. Created by Andrey Vasiliev

FROM ubuntu:18.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get --no-install-recommends -y install git zip unzip xz-utils wget ca-certificates \
    && apt-get clean

ARG CMAKE_VERSION=3.19.8
ARG CMAKE_HOME=/opt/cmake-${CMAKE_VERSION}
ARG NINJA_VERSION=1.10.2

RUN wget https://github.com/ninja-build/ninja/releases/download/v${NINJA_VERSION}/ninja-linux.zip -P /tmp \
    && unzip /tmp/ninja-linux.zip -d /usr/local/bin \
    && rm /tmp/ninja-linux.zip \
    && wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh -O /tmp/cmake-install.sh \
    && chmod u+x /tmp/cmake-install.sh \
    && mkdir ${CMAKE_HOME} \
    && /tmp/cmake-install.sh --skip-license --prefix=${CMAKE_HOME} \
    && rm /tmp/cmake-install.sh

ENV PATH="${CMAKE_HOME}/bin:${PATH}"

ARG ANDROID_HOME=/opt/android-sdk

WORKDIR /opt

RUN apt-get update \
    && apt-get -y install --no-install-recommends openjdk-8-jdk \
    && apt-get clean \
    && wget https://dl.google.com/android/repository/commandlinetools-linux-7583922_latest.zip -O tools.zip \
    && unzip tools.zip \
    && rm tools.zip \
    && cd cmdline-tools/bin \
    && yes | ./sdkmanager  --licenses --sdk_root=${ANDROID_HOME} \
    && rm -rf /root/.android /root/.gradle

ENV PATH="/opt/tools/bin:${PATH}"
ENV ANDROID_SDK_ROOT="${ANDROID_HOME}"

#ARG GRADLE_VERSION=7.3.2
#
#RUN wget https://services.gradle.org/distributions/gradle-${GRADLE_VERSION}-bin.zip \
#    && unzip gradle-${GRADLE_VERSION}-bin.zip \
#    && rm gradle-${GRADLE_VERSION}-bin.zip \
#    && ./gradle-${GRADLE_VERSION}/bin/gradle
#
#ENV PATH="${PATH}:/opt/gradle-${GRADLE_VERSION}/bin"
