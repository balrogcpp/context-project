#!/bin/bash

mkdir artifacts
docker build -t cross_test . &&\
docker run -it -d cross_test &&\
CONTAINER_ID=$(docker ps -alq)
GIT_SHA1=$(git rev-parse --short=8 HEAD)
docker cp $CONTAINER_ID:/mnt/artifacts ./
docker stop $CONTAINER_ID
