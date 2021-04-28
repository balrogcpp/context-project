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

FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive


ENV ANDROID_HOME=/opt/android-sdk

RUN apt-get update \
    && apt-get install -y git wget zip openjdk-8-jdk \
    && apt-get clean

WORKDIR /opt


########################################################################################################################
RUN wget https://dl.google.com/android/repository/commandlinetools-linux-6609375_latest.zip -O tools.zip \
    && unzip tools.zip \
    && rm tools.zip \
    && cd tools/bin \
    && export ANDROID_HOME=/opt/android-sdk \
    && yes | ./sdkmanager  --licenses --sdk_root=${ANDROID_HOME} \
    && cd ../

ENV PATH="/opt/tools/bin:${PATH}"
ENV ANDROID_SDK_ROOT="${ANDROID_HOME}"
