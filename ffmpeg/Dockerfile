# BUILD STAGE
FROM ubuntu:20.04 AS build-stage

ARG DIR=/tmp/build
ARG PREFIX=/usr/local

WORKDIR ${DIR}

ENV TZ=Europe/Paris
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN buildDeps="autoconf \
  automake \
  build-essential \
  cmake \
  libass-dev \
  libfreetype6-dev \
  libgnutls28-dev \
  libsdl2-dev \
  libtool \
  libva-dev \
  libvdpau-dev \
  libvorbis-dev \
  libxcb1-dev \
  libxcb-shm0-dev \
  libxcb-xfixes0-dev \
  libx264-dev \
  libx265-dev \
  libnuma-dev \
  libfdk-aac-dev \
  libmp3lame-dev \
  pkg-config \
  texinfo \
  wget \
  yasm \
  zlib1g-dev \
  libssl-dev \
  tclsh \
  git \
  ca-certificates" && \
apt-get -yqq update && \
apt-get install -yq --no-install-recommends ${buildDeps} 

# Copying blackmagic SDK into source folder
RUN mkdir -p ./ffmpeg_sources 
COPY ./ffmpeg_sources ./ffmpeg_sources

# Libsrt sources
RUN cd ./ffmpeg_sources && \
    git clone https://github.com/Haivision/srt.git && \
    cd ./srt && \
    ./configure --prefix="${PREFIX}" && \
    make && make install

# Downloading ffmpeg and extracting it
RUN cd ./ffmpeg_sources && \
    wget -O ffmpeg-snapshot.tar.bz2 https://ffmpeg.org/releases/ffmpeg-snapshot.tar.bz2 && \
    tar xjvf ffmpeg-snapshot.tar.bz2 && \
    cd ./ffmpeg

# FFmpeg Configure
RUN cd ./ffmpeg_sources/ffmpeg && \ 
    export PATH="${PREFIX}/bin:$PATH" PKG_CONFIG_PATH="${PREFIX}/lib/pkgconfig:${PREFIX}/lib64/pkgconfig:$PATH" && \
    ./configure \
        --prefix="${PREFIX}" \
        --extra-cflags="-I${PREFIX}/include -I${DIR}/ffmpeg_sources/BMD_SDK/include" \
        --extra-ldflags="-L${PREFIX}/lib -L${PREFIX}/lib64" \
        --extra-libs="-lpthread -lm" \
        --bindir="${PREFIX}/bin" \
        --enable-gpl \
        --enable-shared \
        --enable-libmp3lame \
        --enable-gnutls \
        --enable-libass \
        --enable-libfdk-aac \
        --enable-libfreetype \
        --enable-libvorbis \
        --enable-libx264 \
        --enable-libx265 \
        --enable-nonfree \ 
        --enable-decklink \
        --enable-libsrt 

# Compile FFmpeg
RUN bash && \
    cd ./ffmpeg_sources/ffmpeg && \
    make -j4 && \
    make install && \
    make distclean && \
    hash -r

# Clean before going to release stage
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${PREFIX}/lib:${PREFIX}/lib64
RUN ldd ${PREFIX}/bin/ffmpeg | grep /lib/x86_64-linux-gnu | cut -d ' ' -f 3 | xargs -i cp {} ${PREFIX}/lib/ && \
    rm -rf ${DIR} && \
    rm -rf /var/lib/apt/lists/* && \
    LD_LIBRARY_PATH=/usr/local/lib ffmpeg -buildconf


# RELEASE STAGE
FROM ubuntu:20.04 AS release-stage

ARG DIR=/usr/local


WORKDIR ${DIR}

COPY --from=build-stage ${DIR} .

ENV PATH=${DIR}/bin:$PATH 
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${DIR}/lib:${DIR}/lib64

# Dependencies for ssh setup
# RUN apk add --no-cache openssh-server openrc
RUN sshDeps="openssh-server \
    ca-certificates" && \
    apt-get -yqq update && \
    apt-get install -yq --no-install-recommends ${sshDeps} && \
    rm -rf /var/lib/apt/lists/*

# Copying ssh scripts into container
COPY ./scripts ./scripts
RUN chmod +x ./scripts

COPY ./sources ./sources

# Preparing user for ssh access
RUN adduser ffmpeg --disabled-password && \
    echo 'ffmpeg:root' | chpasswd

ENTRYPOINT [  "./scripts/entrypoint.sh"  ]
