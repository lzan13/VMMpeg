//
// Created by lz liu on 2018/5/11.
//
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
#include "VMFilter.h"

/**
 * 滤镜
 * @param env
 * @param type
 * @param data_
 * @param width
 * @param height
 * @return
 */
JNIEXPORT jint JNICALL
Java_com_vmloft_develop_library_ffmpeg_VMFilter_testFilter(JNIEnv *env, jclass type, jbyteArray data_, jint width, jint height) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);
    int ret = testFilter((uint8_t *) data, width, height);
    env->ReleaseByteArrayElements(data_, data, 0);
    return ret;
}
}