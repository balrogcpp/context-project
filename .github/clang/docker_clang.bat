@echo off
setlocal EnableDelayedExpansion

docker build -t balrogcpp/clang_cross .
REM docker push balrogcpp/clang_cross
