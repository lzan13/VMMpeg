#!/bin/bash

# 编译临时目录
export TMPDIR=$(pwd)/tmp
# 编译输出前缀，就是制定编译输出的 so 和头文件路径
export PREFIX=$(pwd)/x264
# NDK 路径
NDK=/Users/lzan13/develop/android/android-ndk
# API 版本
API=21
# 编译生成的库支持的 CPU 架构
CPU=armv7-a
# 编译针对的平台前缀，API表示支持的版本
PLATFORM=${NDK}/platforms/android-$API/arch-arm
# 工具链的路径，根据编译的平台不同而不同 arm-linux-androideabi-4.9与上面设置的PLATFORM对应，4.9为工具的版本号，根据自己安装的NDK版本来确定，一般使用最新的版本
TOOLCHAIN=${NDK}/toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64
# 头文件地址，因为新版 NDK 将 include 都移动到了 sysroot 下，所以需要进行配置，如果是使用 10e版本不需要此配置
ISYSROOT=${NDK}/sysroot/usr/include
# 优化参数
OPTIMIZE_CFLAGS=""
ADDI_CFLAGS=""

# 输出参数看下是否正确
echo "build tmp:        ${TMPDIR}"
echo "x264 prefix:      ${PREFIX}"
echo "x264 optimize1:   ${OPTIMIZE_CFLAGS}"
echo "x264 optimize2:   ${ADDI_CFLAGS}"

# 切到 FFmpeg 目录下
cd ../X264

function build_x264 {
    ./configure \
    --prefix=$PREFIX \
    --enable-static \
    --enable-pic \
    --disable-asm \
    --disable-cli \
    --host=arm-linux-androideabi \
    --cross-prefix=${TOOLCHAIN}/bin/arm-linux-androideabi- \
    --sysroot=${PLATFORM}
    --extra-cflags="${ADDI_CFLAGS} ${OPTIMIZE_CFLAGS}" \

    make clean
    make -j4
    make install
}
build_x264