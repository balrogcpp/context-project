#!/bin/bash

docker build . -f Docker/android/Dockerfile -t balrogcpp/android-ndk &&\
docker login &&\
docker push balrogcpp/android-ndk

exit 0
