FROM ubuntu:22.10

RUN echo "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy main" >> /etc/apt/sources.list
RUN echo "deb-src http://apt.llvm.org/jammy/ llvm-toolchain-jammy main" >> /etc/apt/sources.list
RUN apt-get update && apt-get install -y gdb
RUN apt-get upgrade -y && apt-get install clang-13 valgrind -y
RUN apt-get install time
RUN apt-get install bash-completion -y
RUN apt-get install meson -y
RUN apt-get install ninja-build -y
RUN apt-get install llvm-13 -y
RUN update-alternatives --install \
    /usr/bin/llvm-config       llvm-config      /usr/bin/llvm-config-13  200