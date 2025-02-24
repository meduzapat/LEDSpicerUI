# This docker container can be used to run the tests, it simulates the github actions environment, to resolve errors.
FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
  build-essential  \
  cmake            \
  libgtkmm-3.0-dev \
  libtinyxml2-dev  \
  libgtest-dev     \
  git              \
  xvfb             \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . /app

# Prepare the build environment
RUN mkdir target \
  cmake .. -DCMAKE_CXX_FLAGS="-g3 -O0" \
  cmake --build target --target DefaultsTest --target DataTest --target MessageTest --target XMLHelperTest

# Default command: run tests
CMD cd target && export DISPLAY=:99 && xvfb-run --auto-servernum ctest --output-on-failure
