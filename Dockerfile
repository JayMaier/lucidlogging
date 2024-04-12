FROM arm64v8/ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libudev-dev \
    libgtk-3-dev \
    tmux \
    vim \
    net-tools

RUN mkdir -p /lucidlogging

COPY . /lucidlogging/

RUN tar -xvzf ./lucidlogging/libraries/ArenaSDK_v0.1.49_Linux_ARM64.tar.gz -C ./lucidlogging

WORKDIR /lucidlogging

RUN cd ./ArenaSDK_Linux_ARM64 && sh Arena_SDK_ARM64.conf && cd ..


RUN chmod +x tmux_runner.sh

CMD ["/bin/bash", "tmux_runner.sh"]
