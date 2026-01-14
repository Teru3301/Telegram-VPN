# =========================
# Base: C++ build tools
# =========================
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    curl \
    pkg-config \
    ninja-build \
    libssl-dev \
    libcurl4-openssl-dev \
    libboost-all-dev \
    libconfig++-dev \
    libjsoncpp-dev \
    zlib1g-dev \
    ca-certificates \
    gnupg \
    lsb-release \
    software-properties-common \
    vim \
    less \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
RUN ldconfig

# =========================
# MongoDB C dependencies
# =========================
RUN apt-get update && apt-get install -y \
    libmongoc-dev \
    libbson-dev \
    libmongocrypt-dev \
    && rm -rf /var/lib/apt/lists/*

# =========================
# MongoDB C++ driver (3.9.0)
# =========================
WORKDIR /tmp
RUN git clone https://github.com/mongodb/mongo-cxx-driver \
    && cd mongo-cxx-driver \
    && git checkout r3.9.0 \
    && mkdir -p build \
    && cd build \
    && cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr/local \
        -DBUILD_VERSION=3.9.0 \
        -DENABLE_TESTS=OFF \
    && make -j$(nproc) \
    && make install \
    && rm -rf /tmp/mongo-cxx-driver

RUN ldconfig

# =========================
# TgBot (tgbot-cpp)
# =========================
WORKDIR /tmp
RUN git clone https://github.com/reo7sp/tgbot-cpp \
    && cd tgbot-cpp \
    && mkdir -p build \
    && cd build \
    && cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DENABLE_TESTS=OFF \
    && make -j$(nproc) \
    && make install \
    && rm -rf /tmp/tgbot-cpp

RUN ldconfig

# =========================
# Your Telegram VPN Bot
# =========================
WORKDIR /app
COPY Bot/ /app/Bot/

# Fix MongoDB 3.9.0 API incompatibilities
RUN sed -i 's/return result\.has_value();/return static_cast<bool>(result);/' \
    /app/Bot/src/mongo/config.cpp

RUN find /app/Bot -type f \( -name "*.cpp" -o -name "*.hpp" \) \
    -exec grep -l "\.has_value()" {} \; \
    | xargs -I {} sed -i 's/\.has_value()//g' {} || true

# Build project
WORKDIR /app/Bot
RUN mkdir -p build \
    && cd build \
    && cmake .. \
    && make -j$(nproc)

# Runtime setup
WORKDIR /app
RUN mkdir -p /app/config \
    && rm -rf /tmp/*

CMD ["/app/Bot/build/bot"]

