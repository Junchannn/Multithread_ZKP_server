# Use Ubuntu 20.04 as the base image for compatibility with g++-10
FROM ubuntu:20.04

ENV NTL_VERSION 11.4.1
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies, including g++-10, GMP, and libc6-dev for system headers
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    g++-10 \
    nlohmann-json3-dev \
    wget \
    make \
    libgmp-dev \
    libc6-dev \
    linux-libc-dev && \
    rm -rf /var/lib/apt/lists/* && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 10 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 10

# Download, extract, and install NTL library with --no-check-certificate option
RUN set -ex && \
    wget --no-check-certificate -O ntl.tar.gz "https://www.shoup.net/ntl/ntl-$NTL_VERSION.tar.gz" && \
    mkdir -p /usr/src/ntl && \
    tar -xzC /usr/src/ntl --strip-components=1 -f ntl.tar.gz && \
    rm ntl.tar.gz && \
    cd /usr/src/ntl/src && \
    ./configure CXXFLAGS="-g -O2 -fPIC -march=native -pthread -std=c++20" && \
    make && \
    make install && \
    rm -rf /usr/src/ntl

# Create directory structure
RUN mkdir -p /ZKP_server/src/server /ZKP_server/src/ZKP

# Copy project files into the container
COPY src/server/* /ZKP_server/src/server/
COPY src/ZKP/* /ZKP_server/src/ZKP/

# Compile the source files into verifier_server using C++20 and enable GNU extensions, linking against GMP
RUN cd /ZKP_server && g++ -std=c++20 -D_GNU_SOURCE -I/usr/include/nlohmann -I/usr/local/include/NTL -o verifier_server src/server/*.cpp src/ZKP/*.cpp -lntl -lgmp -pthread

# Expose port for the application
EXPOSE 1337

# Set verifier_server as the main process
CMD ["/ZKP_server/verifier_server"]
