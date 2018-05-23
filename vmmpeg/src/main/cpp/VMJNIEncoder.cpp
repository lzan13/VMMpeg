//
// Created by lz liu on 2018/5/11.
//
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
#include "VMLog.h"
#include "VMEncoder.h"


/**
 * 初始化编码器
 * @param outPath_
 * @param width
 * @param height
 * @param rotation
 */
JNIEXPORT jint JNICALL
Java_com_vmloft_develop_library_ffmpeg_VMEncoder_initEncoder(JNIEnv *env, jclass type, jstring outPath_, jint width, jint height, jint rotation) {
    const char *outPath = env->GetStringUTFChars(outPath_, 0);
    int ret = initEncoder(outPath, width, height, rotation);
    env->ReleaseStringUTFChars(outPath_, outPath);
    return ret;
}

/**
 * 编码数据
 * @param data_
 * @param widht
 * @param height
 */
JNIEXPORT jint JNICALL
Java_com_vmloft_develop_library_ffmpeg_VMEncoder_encodeData(JNIEnv *env, jclass type, jbyteArray data_, jint width, jint height, jint rotation) {
    // YUV 数据格式
    // I420: YYYYYYYY UU VV    =>YUV420P
    // YV12: YYYYYYYY VV UU    =>YUV420P
    // NV12: YYYYYYYY UVUV     =>YUV420SP
    // NV21: YYYYYYYY VUVU     =>YUV420SP
    jbyte *data = env->GetByteArrayElements(data_, NULL);
    int ret = encodeData(reinterpret_cast<uint8_t *>(data), width, height, rotation);
    env->ReleaseByteArrayElements(data_, data, 0);
    return ret;
}

/**
 * 释放编码器
 */
JNIEXPORT jint JNICALL
Java_com_vmloft_develop_library_ffmpeg_VMEncoder_freeEncoder(JNIEnv *env, jclass type) {
    return freeEncoder();
}

/**
 * YUV 编码为 图片
 * @param data_ 一帧 yuv 数据
 * @param width 数据宽
 * @param height 数据高
 * @param outPath_ 保存路径
 * @return
 */
JNIEXPORT jint JNICALL
Java_com_vmloft_develop_library_ffmpeg_VMEncoder_yuv2Bitmap(JNIEnv *env, jclass type, jbyteArray data_, jint width, jint height, jint degrees, jstring outPath_) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);
    const char *outPath = env->GetStringUTFChars(outPath_, 0);

    int ret = yuv2Bitmap(reinterpret_cast<uint8_t *>(data), width, height, degrees, outPath);

    env->ReleaseByteArrayElements(data_, data, 0);
    env->ReleaseStringUTFChars(outPath_, outPath);
    return ret;
}
}