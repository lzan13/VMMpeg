//
// Created by lz liu on 2018/6/7.
//
#include <jni.h>

#include "VMLog.h"
#include "VMYuv.h"
#include "VMVideoEncoder.h"

VMVideoEncoder *videoEncoder;

extern "C" {

/**
 * 初始化视频编码器
 * @param inWidth 数据源宽
 * @param inHeight 数据源高
 * @param outWidth 目标数据宽
 * @param outHeight 目标数据高
 * @return
 */
JNIEXPORT jboolean JNICALL
Java_com_vmloft_develop_library_ffmpeg_VMMpegProcess_initVideoEncoder(JNIEnv *env, jclass type, jint inWidth, jint inHeight, jint outWidth, jint outHeight) {

    // 初始化 YUV 工具类
    yuvInit(inWidth, inHeight, outWidth, outHeight);

    videoEncoder = new VMVideoEncoder();
    videoEncoder->setWidth(outWidth);
    videoEncoder->setHeight(outHeight);
    videoEncoder->setBitrate(128);
    videoEncoder->open();
    return 0;
}

/**
 * 编码视频数据
 * @param srcData_ 源数据
 * @param frameSize 源数据大小
 * @param fps 帧
 * @param dstData_ 目标数据
 * @param outFrameSize_  目标数据大小，这里保存的是一个数组，如果编码帧包含 SPS PPS
 * @return
 */
JNIEXPORT jint JNICALL
Java_com_vmloft_develop_library_ffmpeg_VMMpegProcess_encodeVideoData(JNIEnv *env, jclass type, jbyteArray srcData_, jint frameSize, jint fps, jbyteArray dstData_, jintArray outFrameSize_) {
    jbyte *srcData = env->GetByteArrayElements(srcData_, NULL);
    jbyte *dstData = env->GetByteArrayElements(dstData_, NULL);
    jint *outFrameSize = env->GetIntArrayElements(outFrameSize_, NULL);

    int numNals = videoEncoder->encodeFrame((uint8_t *) srcData, frameSize, fps,
                                            (uint8_t *) dstData, outFrameSize);

    env->ReleaseByteArrayElements(srcData_, srcData, 0);
    env->ReleaseByteArrayElements(dstData_, dstData, 0);
    env->ReleaseIntArrayElements(outFrameSize_, outFrameSize, 0);
    return numNals;
}

/**
 * 释放编码处理器
 */
JNIEXPORT jint JNICALL
Java_com_vmloft_develop_library_ffmpeg_VMMpegProcess_release(JNIEnv *env, jclass type) {
    free(videoEncoder);
    release();
    return 0;
}

/**
 * NV21 转 I420
 * @param srcNV21_ 源数据为 NV21 格式
 * @param width 源数据宽
 * @param height 源数据高
 * @param dstData_ 目标数据 I420 格式
 * @return
 */
JNIEXPORT jint JNICALL
Java_com_vmloft_develop_library_ffmpeg_VMMpegProcess_nv21ToI420(JNIEnv *env, jclass type, jbyteArray srcNV21_, jint width, jint height, jbyteArray dstData_) {
    jbyte *srcNV21 = env->GetByteArrayElements(srcNV21_, NULL);
    jbyte *dstData = env->GetByteArrayElements(dstData_, NULL);

    nv21ToI420(reinterpret_cast<uint8_t *>(srcNV21), width, height,
               reinterpret_cast<uint8_t *>(dstData));

    env->ReleaseByteArrayElements(srcNV21_, srcNV21, 0);
    env->ReleaseByteArrayElements(dstData_, dstData, 0);
    return 0;
}

/**
 * 缩放 YUV 数据，这里需要标准的 YUV420P 格式 YUV 数据 如果是 NV21 格式可以调用 nv21ToI420() 进行转换
 *
 * @param srcData_ 源数据为 I420 格式
 * @param width 源宽高
 * @param height
 * @param dstData_ 目标数据
 * @param dstWidth 目标宽高
 * @param dstHeight
 * @param mode 缩放模式，0 为最快，3 为质量最好
 */
JNIEXPORT jint JNICALL
Java_com_vmloft_develop_library_ffmpeg_VMMpegProcess_yuvScale(JNIEnv *env, jclass type, jbyteArray srcData_, jint width, jint height, jbyteArray dstData_, jint dstWidth, jint dstHeight, jint mode) {
    jbyte *srcData = env->GetByteArrayElements(srcData_, NULL);
    jbyte *dstData = env->GetByteArrayElements(dstData_, NULL);

    yuvScale(reinterpret_cast<uint8_t *>(srcData), width, height,
             reinterpret_cast<uint8_t *>(dstData), dstWidth, dstHeight, 0);

    env->ReleaseByteArrayElements(srcData_, srcData, 0);
    env->ReleaseByteArrayElements(dstData_, dstData, 0);
    return 0;
}

/**
 * 旋转 YUV 数据，Android 摄像头一般都会旋转 90 或 270 度
 *
 * @param srcData_ 源数据为 I420 格式
 * @param width 源宽高
 * @param height
 * @param dstData_ 目标数据
 * @param angle 旋转角度
 */
JNIEXPORT jint JNICALL
Java_com_vmloft_develop_library_ffmpeg_VMMpegProcess_yuvRotate(JNIEnv *env, jclass type, jbyteArray srcData_, jint width, jint height, jbyteArray dstData_, jint angle) {
    jbyte *srcData = env->GetByteArrayElements(srcData_, NULL);
    jbyte *dstData = env->GetByteArrayElements(dstData_, NULL);

    yuvRotate(reinterpret_cast<uint8_t *>(srcData), width, height,
              reinterpret_cast<uint8_t *>(dstData), angle);

    env->ReleaseByteArrayElements(srcData_, srcData, 0);
    env->ReleaseByteArrayElements(dstData_, dstData, 0);
    return 0;
}

/**
 * 镜像 YUV 数据，一般前置摄像头会镜像数据，如果想保存的数据正常就需要进行镜像操作
 *
 * @param srcData_ 源数据为 I420 格式
 * @param width 源宽高
 * @param height
 * @param dstData_ 目标数据
 */
JNIEXPORT jint JNICALL
Java_com_vmloft_develop_library_ffmpeg_VMMpegProcess_yuvMirror(JNIEnv *env, jclass type, jbyteArray srcData_, jint width, jint height, jbyteArray dstData_) {
    jbyte *srcData = env->GetByteArrayElements(srcData_, NULL);
    jbyte *dstData = env->GetByteArrayElements(dstData_, NULL);

    yuvMirror(reinterpret_cast<uint8_t *>(srcData), width, height,
              reinterpret_cast<uint8_t *>(dstData));

    env->ReleaseByteArrayElements(srcData_, srcData, 0);
    env->ReleaseByteArrayElements(dstData_, dstData, 0);
    return 0;
}

/**
 * 剪切 YUV 数据
 *
 * @param srcData_ 源数据为 I420 格式
 * @param width 源宽高
 * @param height
 * @param dstData_ 目标数据
 * @param dstWidth 目标宽高
 * @param dstHeight
 * @param left 剪切起点坐标
 * @param top
 */
JNIEXPORT jint JNICALL
Java_com_vmloft_develop_library_ffmpeg_VMMpegProcess_yuvCrop(JNIEnv *env, jclass type, jbyteArray srcData_, jint width, jint height, jbyteArray dstData_, jint dstWidth, jint dstHeight, jint left, jint top) {
    jbyte *srcData = env->GetByteArrayElements(srcData_, NULL);
    jbyte *dstData = env->GetByteArrayElements(dstData_, NULL);

    yuvCrop(reinterpret_cast<uint8_t *>(srcData), width, height,
            reinterpret_cast<uint8_t *>(dstData), dstWidth, dstHeight, left, top);

    env->ReleaseByteArrayElements(srcData_, srcData, 0);
    env->ReleaseByteArrayElements(dstData_, dstData, 0);
    return 0;
}

/**
 * 压缩 YUV 数据，这中间会包括 转换、旋转、缩放、镜像等
 *
 * @param srcData_ 源数据
 * @param width 输入宽高
 * @param height
 * @param dstData_ 目标数据
 * @param dstWidth 目标宽高
 * @param dstHeight
 * @param mode 压缩模式 0 最快 3 质量最好
 * @param angle 旋转角度
 * @param isMirror 是否镜像
 */
JNIEXPORT jint JNICALL
Java_com_vmloft_develop_library_ffmpeg_VMMpegProcess_yuvCompress(JNIEnv *env, jclass type, jbyteArray srcData_, jint width, jint height, jbyteArray dstData_, jint dstWidth, jint dstHeight, jint mode, jint angle, jboolean isMirror) {
    jbyte *srcData = env->GetByteArrayElements(srcData_, NULL);
    jbyte *dstData = env->GetByteArrayElements(dstData_, NULL);

    yuvCompress(reinterpret_cast<uint8_t *>(srcData), width, height,
                reinterpret_cast<uint8_t *>(dstData), dstWidth, dstHeight, mode, angle, isMirror);

    env->ReleaseByteArrayElements(srcData_, srcData, 0);
    env->ReleaseByteArrayElements(dstData_, dstData, 0);
    return 0;
}
}