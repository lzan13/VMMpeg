#!/bin/bash

# ffmpeg configure 命令注释
# --arch=arm \          # 机器设备架构，android 系统采用 arm 架构
# --cpu=${CPU} \        # 编译支持的 CPU 架构
# --prefix=${PREFIX} \  # 编译输出前缀
# --target-os=android \ # 目标平台，这是后可以不用修改 configure 文件
# --enable-shared \     # 生成动态库
# --enable-static \     # 生成静态库，主要是为了后边将多个库打包成一个库
# --enable-gpl \        # 生成 gpl 主要是连接编码器使用，比如 x264/openh264
# --enable-small \      # 生成小的库
# --enable-cross-compile \      # 
# --enable-runtime-cpudetect \  # 启用 CPU 动态监测
# --disable-asm \       # 禁止 asm
# --disable-debug \     # 禁止 debug 库
# --disable-doc \       # 禁止 文档
# --disable-ffmpeg \    # 禁止 ffmpeg
# --disable-ffplay \    # 禁止 ffplay
# --disable-ffprobe \   # 禁止 ffprobe
# --disable-postproc \  # 禁止 postproc
# --disable-programs \  # 禁止 programs
# --disable-avdevice \  # 禁止 avdevices
# --disable-symver \    # 禁止 symver
# --disable-stripping \ # 禁止 stripping
# --libdir=${PREFIX}/libs/armeabi-v7a \             # 定义编译 so 生成文件目录
# --incdir=${PREFIX}/includes/armeabi-v7a \         # 定义编译 .h 头文件生成文件目录
# --pkgconfigdir=${PREFIX}/pkgconfig/armeabi-v7a \  # 定义编译 pkgconfig 生成文件目录
# --cc=${TOOLCHAIN_PREFIX}/arm-linux-androideabi-4.9/prebuilt/${NDK_PLATFORM}/bin/arm-linux-androideabi-gcc \           # 使用的编译器，这里使用 ndk 下的 gcc 工具
# --cross-prefix=${TOOLCHAIN_PREFIX}/arm-linux-androideabi-4.9/prebuilt/${NDK_PLATFORM}/bin/arm-linux-androideabi- \    # 交叉编译工具前缀
# --sysroot=${PLATFORM_PREFIX}/arch-arm \   # 编译针对的平台
# --extra-cflags="-march=${CPU}" \          # 编译优化配置
# --extra-ldexeflags=-pie \ 

# 编译临时目录
export TMPDIR=$(pwd)/tmp
# 编译输出前缀，就是制定编译输出的 so 和头文件路径
export PREFIX=$(pwd)/ffmpeg
# x264静态库及头文件路径
X264LIB=$(pwd)/x264/lib
X264INC=$(pwd)/x264/include
# NDK 路径
NDK=/Users/lzan13/develop/android/android-ndk
# NDK 平台，这里是 mac 平台
NDK_PLATFORM=darwin-x86_64
# API 版本
API=21
# ffmpeg 编译生成的库支持的 CPU 架构
CPU=armv7-a
# ffmpeg arch
ARCH=arm
# 编译针对的平台前缀，API表示支持的版本
PLATFORM_PREFIX=${NDK}/platforms/android-$API
# 工具链的路径，根据编译的平台不同而不同 arm-linux-androideabi-4.9与上面设置的PLATFORM对应，4.9为工具的版本号，根据自己安装的NDK版本来确定，一般使用最新的版本
TOOLCHAIN_PREFIX=${NDK}/toolchains/
# 头文件地址，因为新版 NDK 将 include 都移动到了 sysroot 下，所以需要进行配置，如果是使用 10e版本不需要次配置
ISYSROOT=${NDK}/sysroot/usr/include
# 优化参数
OPTIMIZE_CFLAGS="-mfloat-abi=softfp -mfpu=vfpv3-d16 -mthumb"
ADDI_CFLAGS="-O3 -Wall -pipe \
    -ffast-math \
    -fstrict-aliasing -Werror=strict-aliasing \
    -Wno-psabi -Wa,--noexecstack \
    -DANDROID \
    -I${X264INC}
    "

# 输出参数看下是否正确
echo "build tmp:    ${TMPDIR}"
echo "prefix:       ${PREFIX}"
echo "x264 lib:     ${X264LIB}"
echo "x264 include: ${X264INC}"
echo "optimize1:    ${OPTIMIZE_CFLAGS}"
echo "optimize2:    ${ADDI_CFLAGS}"

# 切到 FFmpeg 目录下
cd ../FFmpeg

COMMON_OPTIONS="\
    --prefix=${PREFIX} \
    --target-os=android \
    --enable-shared \
    --enable-static \
    --enable-small \
    --enable-cross-compile \
    --enable-runtime-cpudetect \
    --disable-avdevice \
    --disable-asm \
    --disable-debug \
    --disable-doc \
    --disable-ffmpeg \
    --disable-ffplay \
    --disable-ffprobe \
    --disable-postproc \
    --disable-programs \
    --disable-symver \
    --disable-stripping \
    --disable-encoders \
    --enable-libx264 \
    --enable-gpl \
    --enable-encoder=libx264 \
    --enable-encoder=acc \
    --enable-encoder=mjpeg \
    --enable-encoder=png \
    --disable-decoders \
    --enable-decoder=aac \
    --enable-decoder=aac_latm \
    --enable-decoder=h264 \
    --enable-decoder=mpeg4 \
    --enable-decoder=mjpeg \
    --enable-decoder=png \
    --disable-demuxers \
    --enable-demuxer=image2 \
    --enable-demuxer=h264 \
    --enable-demuxer=aac \
    --enable-demuxer=avi \
    --enable-demuxer=mpc \
    --enable-demuxer=mov \
    --disable-parsers \
    --enable-parser=aac \
    --enable-parser=ac3 \
    --enable-parser=h264 \
    "

function build_android {
    # 编译 armeabi-v7a 架构库
    CPU=armv7-a
    ARCH=arm
    ./configure \
    --arch=${ARCH} \
    --cpu=${CPU} \
    --libdir=${PREFIX}/libs/armeabi-v7a \
    --incdir=${PREFIX}/includes/armeabi-v7a \
    --pkgconfigdir=${PREFIX}/pkgconfig/armeabi-v7a \
    --cc=${TOOLCHAIN_PREFIX}/arm-linux-androideabi-4.9/prebuilt/${NDK_PLATFORM}/bin/arm-linux-androideabi-gcc \
    --cross-prefix=${TOOLCHAIN_PREFIX}/arm-linux-androideabi-4.9/prebuilt/${NDK_PLATFORM}/bin/arm-linux-androideabi- \
    --sysroot=${PLATFORM_PREFIX}/arch-arm \
    --extra-cflags="${ADDI_CFLAGS} ${OPTIMIZE_CFLAGS} -march=${CPU}" \
    --extra-ldflags="-L${X264LIB} -lx264" 
    ${COMMON_OPTIONS}
    make clean
    make -j4 && make install

    # 编译 arm64-v8a 架构库
    CPU=armv8-a
    ARCH=aarch64
    ./configure \
    --arch=${ARCH} \
    --cpu=${CPU}\
    --libdir=${PREFIX}/libs/arm64-v8a \
    --incdir=${PREFIX}/includes/arm64-v8a \
    --pkgconfigdir=${PREFIX}/pkgconfig/arm64-v8a \
    --cc=${TOOLCHAIN_PREFIX}/aarch64-linux-android-4.9/prebuilt/${NDK_PLATFORM}/bin/aarch64-linux-android-gcc \
    --cross-prefix=${TOOLCHAIN_PREFIX}/aarch64-linux-android-4.9/prebuilt/${NDK_PLATFORM}/bin/aarch64-linux-android- \
    --sysroot=${PLATFORM_PREFIX}/arch-arm64 \
    --extra-cflags="${ADDI_CFLAGS} ${OPTIMIZE_CFLAGS} -march=${CPU}" \
    --extra-ldflags="-L${X264LIB} -lx264" 
    ${COMMON_OPTIONS}
    make clean
    make -j4 && make install

    # 编译 x86 架构库
    CPU=i686
    ARCH=x86
    ./configure \
    --arch=${ARCH} \
    --cpu=${CPU} \
    --libdir=${PREFIX}/libs/armeabi-v7a \
    --incdir=${PREFIX}/includes/armeabi-v7a \
    --pkgconfigdir=${PREFIX}/pkgconfig/armeabi-v7a \
    --cc=${TOOLCHAIN_PREFIX}/x86-4.9/prebuilt/${NDK_PLATFORM}/bin/x86-linux-android-gcc \
    --cross-prefix=${TOOLCHAIN_PREFIX}/x86-4.9/prebuilt/${NDK_PLATFORM}/bin/x86-linux-android- \
    --sysroot=${PLATFORM_PREFIX}/arch-x86 \
    --extra-cflags="${ADDI_CFLAGS} ${OPTIMIZE_CFLAGS} -march=${CPU}" \
    --extra-ldflags="-L${X264LIB} -lx264" 
    ${COMMON_OPTIONS}
    make clean
    make -j4 && make install

    # 编译 x86_64 架构库
    CPU=x86_64
    ARCH=x86_64
    ./configure \
    --arch=${ARCH} \
    --cpu=${CPU} \
    --libdir=${PREFIX}/libs/armeabi-v7a \
    --incdir=${PREFIX}/includes/armeabi-v7a \
    --pkgconfigdir=${PREFIX}/pkgconfig/armeabi-v7a \
    --cc=${TOOLCHAIN_PREFIX}/x86_64-4.9/prebuilt/${NDK_PLATFORM}/bin/x86_64-linux-android-gcc \
    --cross-prefix=${TOOLCHAIN_PREFIX}/x86_64-4.9/prebuilt/${NDK_PLATFORM}/bin/x86_64-linux-android- \
    --sysroot=${PLATFORM_PREFIX}/arch-x86_64 \
    --extra-cflags="${ADDI_CFLAGS} ${OPTIMIZE_CFLAGS} -march=${CPU}" \
    --extra-ldflags="-L${X264LIB} -lx264" 
    ${COMMON_OPTIONS}
    make clean
    make -j4 && make install
}
build_android