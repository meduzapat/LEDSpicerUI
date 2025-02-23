# This docker container can be used to run the tests, it simulates the github actions environment, to resolve errors.
FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    autoconf \
    automake \
    pkg-config \
    libgtkmm-3.0-dev \
    libtinyxml2-dev \
    libgtest-dev \
    git \
    xvfb \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . /app

# Prepare the build environment
RUN ./autogen.sh && \
    ./configure CXXFLAGS='-g3 -O0'

# Default command: run tests
CMD export DISPLAY=:99 && xvfb-run --auto-servernum make -C tests check
