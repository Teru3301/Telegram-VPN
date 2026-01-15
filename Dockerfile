FROM alpine:3.19

ENV CMAKE_GENERATOR=Ninja
ENV LD_LIBRARY_PATH=/usr/local/lib

RUN apk add --no-cache \
    build-base \
    cmake \
    ninja \
    git \
    curl \
    wget \
    pkgconf \
    openssl-dev \
    curl-dev \
    boost-dev \
    libconfig-dev \
    jsoncpp-dev \
    zlib-dev \
    ca-certificates \
    bash \
    vim \
    less

WORKDIR /workspace

RUN apk add --no-cache \
    mongo-c-driver \
    mongo-c-driver-dev

WORKDIR /tmp
RUN git clone https://github.com/mongodb/mongo-cxx-driver \
    && cd mongo-cxx-driver \
    && git checkout r3.9.0 \
    && mkdir -p build && cd build \
    && cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr/local \
        -DENABLE_TESTS=OFF \
    && ninja \
    && ninja install \
    && rm -rf /tmp/mongo-cxx-driver

WORKDIR /tmp
RUN git clone https://github.com/reo7sp/tgbot-cpp \
    && cd tgbot-cpp \
    && mkdir -p build && cd build \
    && cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DENABLE_TESTS=OFF \
    && ninja \
    && ninja install \
    && rm -rf /tmp/tgbot-cpp

WORKDIR /app
COPY Bot/ /app/Bot/

WORKDIR /app/Bot
RUN mkdir build && cd build \
    && cmake .. \
    && ninja

WORKDIR /app
CMD ["/app/Bot/build/bot"]

