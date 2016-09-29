#!/bin/bash
shopt -s extglob

## Determine path to use as working directory for building external libs
## Create directory for build output if not existing
EXTERNAL_PATH="$( cd "$(dirname "$0")" ; pwd -P )"
BUILD_PATH="$( readlink -m "$EXTERNAL_PATH/../build" )"
[ ! -d "$BUILD_PATH" ] && mkdir "$BUILD_PATH"

source "$EXTERNAL_PATH"/external_utils.sh

########
## Verify prerequisite tools exist
doPrintProgress "Checking required tools"
checkMissingPackages

########
## Build and install all required external libraries
doPrintProgress "Checking required libraries"

if ! fileInstalled "libfreetype.la"; then
  ########
  ## freetype2
  ##
  ## Requires: libharfbuzz
  getArchive http://downloads.sourceforge.net/freetype/freetype-2.7.tar.bz2
  sed -ri "s:.*(AUX_MODULES.*valid):\1:" modules.cfg
  sed -r "s:.*(#.*SUBPIXEL_(RENDERING|HINTING 2)) .*:\1:g" -i include/freetype/config/ftoption.h
  configureBuild --disable-static --with-harfbuzz=no

  has_freetype=yes
else
  has_freetype=yes
fi

if ! fileInstalled "libharfbuzz.la"; then
  ########
  ## harfbuzz
  ##
  ## Requires: libfreetype
  getArchive http://www.freedesktop.org/software/harfbuzz/release/harfbuzz-1.3.1.tar.bz2
  configureBuild

  has_harfbuzz=yes

  if [ "$has_freetype" == "yes" ]; then
    doPrintProgress "Rebuilding FreeType"
    makeClean "freetype-2.7"
    configureBuild --disable-static --with-harfbuzz=yes
  fi
else
  has_harfbuzz=yes
fi

if ! fileInstalled "libz.a"; then
  ########
  ## libz
  ##
  getArchive http://www.zlib.net/zlib-1.2.8.tar.gz
  configureBuild
fi

if ! fileInstalled "libfribidi.la"; then
  ########
  ## libfribidi
  ##
  getArchive http://fribidi.org/download/fribidi-0.19.7.tar.bz2
  configureBuild

  has_fribidi=yes
else
  has_fribidi=yes
fi

if ! fileInstalled "libass.la"; then
  ########
  ## libass
  ##
  ## Requires: libfribidi fontconfig libharfbuzz libfreetype libz
  getGit https://github.com/libass/libass.git
  reconfigureBuild
fi

if ! fileInstalled "libx264.a"; then
  ########
  ## libx264
  ##
  ## See: https://trac.ffmpeg.org/wiki/Encode/H.264
  ## Flags: --enable-gpl --enable-libx264
  getArchive http://download.videolan.org/pub/x264/snapshots/last_x264.tar.bz2
  configureBuild --bindir="$BUILD_PATH/bin" --enable-static --disable-opencl --disable-lavf
fi

if ! fileInstalled "libx265.a"; then
  ########
  ## libx265
  ##
  ## See: https://trac.ffmpeg.org/wiki/Encode/H.265
  ## Tools: cmake mercurial
  getMercurial https://bitbucket.org/multicoreware/x265
  cd build/linux
  cmakeBuild -DENABLE_SHARED:bool=off ../../source
fi

if ! fileInstalled "libfdk-aac.la"; then
  ########
  ## libfdk-aac
  ##
  ## See: https://trac.ffmpeg.org/wiki/Encode/AAC
  ## Flags: --enable-libfdk-aac -enable-nonfree
  getArchive https://github.com/mstorsjo/fdk-aac/tarball/master fdk-aac.tar.gz 
  reconfigureBuild --disable-shared
fi

if ! fileInstalled "libmp3lame.la"; then
  ########
  ## libmp3lame
  ##
  ## Flag: --enable-libmp3lame
  ## Tools: nasm
  getArchive http://downloads.sourceforge.net/project/lame/lame/3.99/lame-3.99.5.tar.gz
  configureBuild --enable-nasm --disable-shared
fi

if ! fileInstalled "libopus.la"; then
  ########
  ## libopus
  ##
  ## Flag: --enable-libopus
  getArchive http://downloads.xiph.org/releases/opus/opus-1.1.2.tar.gz
  configureBuild --disable-shared

  has_opus=yes
else
  has_opus=yes
fi

if ! fileInstalled "libvpx.a"; then
  ########
  ## libvpx
  ##
  ## Flag: --enable-libvpx
  getArchive http://storage.googleapis.com/downloads.webmproject.org/releases/webm/libvpx-1.5.0.tar.bz2 
  configureBuild --disable-examples --disable-unit-tests

  has_vpx=yes
else
  has_vpx=yes
fi

if ! fileInstalled "libsqlite3.la"; then
  ########
  ## libsqlite3
  ##
  getArchive  http://sqlite.org/2016/sqlite-autoconf-3140200.tar.gz
  configureBuild --disable-static        \
            CFLAGS="-g -O2 -DSQLITE_ENABLE_FTS3=1 \
            -DSQLITE_ENABLE_COLUMN_METADATA=1     \
            -DSQLITE_ENABLE_UNLOCK_NOTIFY=1       \
            -DSQLITE_SECURE_DELETE=1              \
            -DSQLITE_ENABLE_DBSTAT_VTAB=1         \
            -DSQLITE_ENABLE_FTS3=1                \
            -DSQLITE_ENABLE_FTS3_PARENTHESIS=1"

########
## ffmpeg
##
if ! fileInstalled "libavcodec.a"; then
  getArchive http://ffmpeg.org/releases/ffmpeg-snapshot.tar.bz2
  PATH="$BUILD_PATH/bin:$PATH" PKG_CONFIG_PATH="$BUILD_PATH/lib/pkgconfig" ./configure \
    --prefix="$BUILD_PATH" \
    --pkg-config-flags="--static" \
    --extra-cflags="-I$BUILD_PATH/include" \
    --extra-ldflags="-L$BUILD_PATH/lib" \
    --bindir="$BUILD_PATH/bin" \
    --enable-gpl \
    --enable-libass \
    --enable-libfdk-aac \
    --enable-libfreetype \
    --enable-libmp3lame \
    --enable-libopus \
    --enable-libtheora \
    --enable-libvorbis \
    --enable-libvpx \
    --enable-libx264 \
    --enable-libx265 \
    --enable-nonfree  >/dev/null 2>&1 || doFailure
  PATH="$BUILD_PATH/bin:$PATH" make >/dev/null 2>&1 || doFailure
  make install  >/dev/null 2>&1 || doFailure
  ## make distclean
  ## hash -r
fi
