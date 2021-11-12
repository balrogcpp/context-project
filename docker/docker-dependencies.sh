#!/bin/bash

docker build .. -f deps-linux/Dockerfile -t balrogcpp/deps-linux \
&& docker build .. -f deps-cross/Dockerfile -t balrogcpp/deps-cross

exit 0
