# 设置构建原生库所需用到的 CMake 最小版本，确保构建时可以使用一组特定的 CMake 功能
cmake_minimum_required(VERSION 3.4.1)

# 设置源文件路径
set(SRC_FILE src/main/cpp)
aux_source_directory(${SRC_FILE} src_files)
MESSAGE("Source file: ${src_files} to build now...")

# 设置 ffmpeg 静态库路径
set(FFMPEG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/ffmpeg)

# 设置编译选项
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

# -s选项为strip，不strip生成的库文件会很大
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s -lm -lz -ldl")

# 定义自己的库名称
add_library(vmmpeg SHARED ${src_files})

# 指定头文件相对路径
include_directories(
    ${FFMPEG_DIR}/include
    src/main/cpp/libyuv/include
    src/main/cpp/include
    )

# 设置 libyuv 编译文件路径
add_subdirectory(src/main/cpp/libyuv ./build)

# 添加引用第三方库

add_library(avfilter STATIC IMPORTED)
set_target_properties(avfilter PROPERTIES IMPORTED_LOCATION ${FFMPEG_DIR}/libs/libavfilter.a)

add_library(avformat STATIC IMPORTED)
set_target_properties(avformat PROPERTIES IMPORTED_LOCATION ${FFMPEG_DIR}/libs/libavformat.a)

add_library(avcodec STATIC IMPORTED)
set_target_properties(avcodec PROPERTIES IMPORTED_LOCATION ${FFMPEG_DIR}/libs/libavcodec.a)

add_library(avutil STATIC IMPORTED)
set_target_properties(avutil PROPERTIES IMPORTED_LOCATION ${FFMPEG_DIR}/libs/libavutil.a)

add_library(swresample STATIC IMPORTED)
set_target_properties(swresample PROPERTIES IMPORTED_LOCATION ${FFMPEG_DIR}/libs/libswresample.a)

add_library(swscale STATIC IMPORTED)
set_target_properties(swscale PROPERTIES IMPORTED_LOCATION ${FFMPEG_DIR}/libs/libswscale.a)

add_library(x264 STATIC IMPORTED)
set_target_properties(x264 PROPERTIES IMPORTED_LOCATION ${FFMPEG_DIR}/libs/libx264.a)

# add_library(yuv STATIC IMPORTED)
# set_target_properties(libyuv PROPERTIES IMPORTED_LOCATION $()/build/libyuv.a)

add_library(postproc STATIC IMPORTED)
set_target_properties(postproc PROPERTIES IMPORTED_LOCATION ${FFMPEG_DIR}/libs/libpostproc.a)

add_library(polarssl STATIC IMPORTED)
set_target_properties(polarssl PROPERTIES IMPORTED_LOCATION ${FFMPEG_DIR}/libs/libpolarssl.a)

add_library(rtmp STATIC IMPORTED)
set_target_properties(rtmp PROPERTIES IMPORTED_LOCATION ${FFMPEG_DIR}/libs/librtmp.a)

# 查找 ndk 自带的一些库，可以给这些库定义别名
find_library(logger log)


# 连接库，其中包含引入的第三方的静态以及动态库
target_link_libraries(vmmpeg avfilter avformat avcodec avutil swresample swscale x264 yuv postproc polarssl rtmp ${logger} )


############################################################################################
#
# CMakeLists.txt 简单流程注释
#
############################################################################################

# add_library 方法可以添加自己编写源文件或一些其他的库
# 当添加自己编写的源文件时，需要设置原生库名称，以及当前库是 STATIC 还是 SHARED，然后就是源文件路径，
# 可以使用多个 add_library 来定义多个库，剩下的一切都有 CMake 去做，在编译项目时会自动打包到 apk 中
# 添加本地原生库，PS 这里需要注意一点：后边源码路径必须带上文件名，只是写文件夹路径不行


# 包含本地原生代码
# add_library(gluelib STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)


# 添加其他预构建库
# 添加预构建库的方法与指定要构建一个原生库类似，不过由于库已经预先构建，需要使用 IMPORTED 标志告知 CMake 我们只希望将库导入到项目中
# 然后我们需要通过 set_target_properties() 命令指定库的路径
# 某些库为特定的 CPU 架构（或应用二进制接口 (ABI)）提供了单独的软件包，并将其组织到单独的目录中。
# 此方法既有助于库充分利用特定的 CPU 架构，又能让我们仅使用所需的库版本，要向 CMake 构建脚本中添加库的多个 ABI 版本，
# 而不必为库的每个版本编写多个命令，我们可以使用 ANDROID_ABI 路径变量，此变量使用 NDK 支持的一组默认 ABI，
# 或者手动配置 Gradle 而让其使用的一组经过筛选的 ABI
#
# add_library(importedlib SHARED IMPORTED)
# set_target_properties(importedlib PROPERTIES IMPORTED_LOCATION importedlib/src/${ANDROID_ABI}/libimportedlib.so)


# find_library({库别名} {NDK 提供的库}) 定义库的别名
# NDK 提供了一套实用的 API 和库，我们只需要简单的将这些已有的库引用到 CMakeLists 脚本文件中就可以使用，
# find_library 方法可以简单的将指定的库文件存储为我们定义的变量，我们可以直接根据此变量引用 NDK 包含的库
# 引用 NDK 包含的一些库，这里有两个参数，下边是以 log 这个库为例，这里保存成 logger
#   1、定义存储 NDK 库的位置的变量名称
#   2、指定 CMake 需要查找的 NDK 库的名称


# 所有通过 add_library() 以及 find_library() 添加的外部库或者本地原生代码，
# 最后都要通过 target_link_libraries() 方法与我们自己的库链接起来
#
# target_link_libraries({自己的库} {需要引用的库}...) 引用 NDK 提供的库
# 为了确保我们的原生库可以调用 log 库中的函数，需要使用 CMake 构建脚本中的 target_link_libraries() 命令关联库
# target_link_libraries 此方法可以将我们的库连接到一个或者多个其他库
# 此方法最少有两个参数，可以更多
#   1、指定需要连接的库
#   2、将 NDK 已有的库连接到我们自己的库