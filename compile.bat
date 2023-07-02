@echo off
setlocal EnableDelayedExpansion

SET _INTERPOLATION_0=
FOR /f "delims=" %%a in ('git rev-parse --short=8 HEAD') DO (SET "_INTERPOLATION_0=!_INTERPOLATION_0! %%a")
SET "GIT_SHA1=!_INTERPOLATION_0!"
docker build -t cross_test . && docker run -it -d cross_test && SET _INTERPOLATION_1=
FOR /f "delims=" %%a in ('docker ps -alq') DO (SET "_INTERPOLATION_1=!_INTERPOLATION_1! %%a")
SET CONTAINER_ID=!_INTERPOLATION_1!
docker cp !CONTAINER_ID!:\var\build\artifacts ./
docker stop !CONTAINER_ID!
