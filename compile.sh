#!/bin/bash

GIT_SHA1=$(git rev-parse --short=8 HEAD)
docker build --build-arg GIT_SHA=$GIT_SHA1 -t cross_test . &&\
docker run -it -d cross_test &&\
CONTAINER_ID=$(docker ps -alq)
docker cp $CONTAINER_ID:/var/build/artifacts ./
docker stop $CONTAINER_ID
