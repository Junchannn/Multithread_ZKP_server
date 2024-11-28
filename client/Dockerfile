# Use Ubuntu 20.04 as the base image for compatibility with g++-10
FROM ubuntu:20.04

ENV NTL_VERSION=11.4.1
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies, including g++-10, GMP, libc6-dev, and Python3
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    g++-10 \
    nlohmann-json3-dev \
    wget \
    make \
    libgmp-dev \
    libc6-dev \
    linux-libc-dev \
    python3 \
    python3-pip && \
    rm -rf /var/lib/apt/lists/* && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 10 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 10 && \
    mkdir -p /ZKP_client/client /ZKP_client/ZKP /ZKP_client/data

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
RUN touch /ZKP_client/data/waiting_time.txt

# Copy project files into the container (adjusted paths)
COPY client/* /ZKP_client/client/
COPY ZKP/* /ZKP_client/ZKP/
COPY data/run_sample_server.py /ZKP_client/data/

# Compile the source files into prover_client using C++20 and enable GNU extensions, linking against GMP
RUN cd /ZKP_client && g++ -std=c++20 -D_GNU_SOURCE -I/usr/include/nlohmann -I/usr/local/include/NTL -o prover_client client/*.cpp ZKP/*.cpp -lntl -lgmp -pthread
RUN pip3 install tqdm
# Expose port for the application (if needed)
EXPOSE 1337

# Set run_sample_server.py as the main process
CMD ["python3","/ZKP_client/data/run_sample_server.py","1000"]
