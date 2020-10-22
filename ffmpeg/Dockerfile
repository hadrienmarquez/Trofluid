# BUILD STAGE
FROM alpine:3.12 as build-stage

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
RUN mkdir -p ./ffmpeg_sources ./ffmpeg_build ./bin

COPY ./ffmpeg_sources ./ffmpeg_sources

# Libsrt sources
RUN cd ./ffmpeg_sources && \
    git clone https://github.com/Haivision/srt.git && \
    cd ./srt && \
    ./configure --prefix="${DIR}/ffmpeg_build"  && \
    make && make install

# Downloading ffmpeg and extracting it
RUN cd ./ffmpeg_sources && \
    wget -O ffmpeg-snapshot.tar.bz2 https://ffmpeg.org/releases/ffmpeg-snapshot.tar.bz2 && \
    tar xjvf ffmpeg-snapshot.tar.bz2 && \
    cd ./ffmpeg

RUN cd ./ffmpeg_sources/ffmpeg && \ 
    export PATH="${DIR}/bin:$PATH" PKG_CONFIG_PATH="${DIR}/ffmpeg_build/lib/pkgconfig:${DIR}/ffmpeg_sources/srt:$PATH" && \
    ./configure \
        --prefix="${DIR}/ffmpeg_build" \
        --extra-cflags="-I${DIR}/ffmpeg_build/include -I${DIR}/ffmpeg_sources/BMD_SDK/include" \
        --extra-ldflags="-L${DIR}/ffmpeg_build/lib" \
        --extra-libs="-lpthread -lm" \
        --bindir="${DIR}/bin" \
        --enable-gpl \
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

RUN bash && \
    cd ./ffmpeg_sources/ffmpeg && \
    make -j `nproc` && \
    make install && \
    hash -r

ENV PATH="${DIR}/bin:$PATH" 
ENV LD_LIBRARY_PATH="${DIR}/ffmpeg_sources/srt"

ENTRYPOINT ["bash"]