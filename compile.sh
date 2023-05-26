#!/bin/bash

GIT_SHA1=$(git rev-parse --short=8 HEAD)
mkdir -p artifacts &&\
docker build -t cross_test . &&\
docker run -it -d cross_test &&\
CONTAINER_ID=$(docker ps -alq)
docker cp $CONTAINER_ID:/var/build/artifacts ./
docker stop $CONTAINER_ID
