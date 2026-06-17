# syntax=docker/dockerfile:1

FROM ubuntu:24.04 AS builder

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        git \
        libssl-dev \
        ca-certificates \
        curl \
        xz-utils \
    && rm -rf /var/lib/apt/lists/*

ENV CMAKE_VERSION=3.30.5

RUN set -eux; \
    arch="$(uname -m)"; \
    case "$arch" in \
      x86_64) pkg="cmake-${CMAKE_VERSION}-linux-x86_64" ;; \
      aarch64|arm64) pkg="cmake-${CMAKE_VERSION}-linux-aarch64" ;; \
      *) echo "Unsupported arch: $arch"; exit 1 ;; \
    esac; \
    curl -fsSL "https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/${pkg}.tar.gz" -o /tmp/cmake.tar.gz; \
    tar -xzf /tmp/cmake.tar.gz -C /opt; \
    ln -s "/opt/${pkg}/bin/cmake" /usr/local/bin/cmake; \
    ln -s "/opt/${pkg}/bin/ctest" /usr/local/bin/ctest; \
    rm /tmp/cmake.tar.gz

WORKDIR /src
COPY . .

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON \
    && cmake --build build -j \
    && ctest --test-dir build --output-on-failure

FROM builder AS tester
WORKDIR /src/build
ENTRYPOINT ["ctest", "--output-on-failure"]

FROM ubuntu:24.04 AS runtime

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        libssl3 \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /src/build/crypto_app /usr/local/bin/crypto_app
WORKDIR /app
ENTRYPOINT ["/usr/local/bin/crypto_app"]
