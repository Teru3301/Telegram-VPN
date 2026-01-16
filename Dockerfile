FROM alpine:3.19 AS builder

RUN apk add --no-cache \
    build-base \
    cmake \
    ninja \
    git \
    boost-dev \
    libconfig-dev \
    jsoncpp-dev \
    zlib-dev \
    openssl-dev \
    curl-dev \
    pkgconf

WORKDIR /tmp

RUN git clone --branch r4.0.0 --depth 1 https://github.com/mongodb/mongo-cxx-driver \
    && cmake -S mongo-cxx-driver -B mongo-cxx-driver/build \
        -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr/local \
        -DENABLE_TESTS=OFF \
        -DBUILD_SHARED_LIBS=ON \
        -DBSONCXX_POLY_USE_MNMLSTC=1 \
    && ninja -C mongo-cxx-driver/build \
    && ninja -C mongo-cxx-driver/build install \
    && rm -rf /tmp/mongo-cxx-driver

RUN git clone --depth 1 https://github.com/reo7sp/tgbot-cpp \
    && cmake -S tgbot-cpp -B tgbot-cpp/build \
        -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DENABLE_TESTS=OFF \
    && ninja -C tgbot-cpp/build \
    && ninja -C tgbot-cpp/build install \
    && rm -rf /tmp/tgbot-cpp

WORKDIR /app/Bot
COPY Bot/ /app/Bot/

RUN cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
    && ninja -C build \
    && strip build/bot

FROM alpine:3.19

RUN apk add --no-cache \
    boost \
    libconfig \
    jsoncpp \
    zlib \
    openssl \
    curl \
    ca-certificates

COPY --from=builder /usr/local/lib /usr/local/lib
COPY --from=builder /usr/local/include /usr/local/include

ENV LD_LIBRARY_PATH=/usr/local/lib

WORKDIR /app

COPY --from=builder /app/Bot/build/bot /app/bot

CMD ["/app/bot"]

