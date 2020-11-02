# BUILD STAGE
FROM alpine:3.12 AS build-stage

ARG DIR=/tmp/build
ARG PREFIX=/usr/local

WORKDIR ${DIR}

RUN apk update && \
    apk upgrade

RUN buildDeps="autoconf \
automake \
bash \
binutils \
bzip2 \
cmake \
git  \
libass-dev \
gnutls-dev \
freetype-dev \
lame-dev \
sdl-dev \
libtool \
libva-dev \
libvdpau-dev \
libvorbis-dev \
x264-dev \
x265-dev \
numactl-dev \
fdk-aac-dev \
coreutils \
diffutils \
expat-dev \
file \
g++ \
gcc \
gperf \
libtool \
make \
tar \
tcl \
texinfo \
openssl-dev \
wget \
yasm \
zlib-dev" && \ 
apk add --no-cache ${buildDeps}

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
    hash -r

# Clean directory before going to release stage
RUN rm -rf ${DIR}

# RELEASE STAGE
FROM build-stage AS release-stage

ARG DIR=/usr/local


WORKDIR ${DIR}

COPY --from=build-stage ${DIR} .

ENV PATH=${DIR}/bin:$PATH 
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${DIR}/lib:${DIR}/lib64

# Copying ssh scripts into container
COPY ./scripts ./scripts
RUN chmod +x ./scripts

# Dependencies for ssh setup
RUN apk add --no-cache openssh-server openrc

# Preparing user for ssh access
RUN adduser ffmpeg --disabled-password && \
    echo 'ffmpeg:root' | chpasswd

ENTRYPOINT [  "./scripts/entrypoint.sh"  ]
