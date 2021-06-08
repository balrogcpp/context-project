FROM balrogcpp/doxygen:latest

WORKDIR /mnt

ADD sources ./sources
ADD doc ./doc
ADD CMakeLists.txt .

RUN mkdir build && cd build && cmake .. && cmake --build . --target pdf && cd ..
