#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "VMLog.h"

extern "C" {
#include "VMFFmpeg.h"

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>

/**
 * 将 byte 数组内容写入到文件
 * @param data_
 * @param filepath_
 */
JNIEXPORT jint JNICALL Java_com_vmloft_develop_library_ffmpeg_VMFFmpeg_saveData(JNIEnv *env, jclass type, jbyteArray data_, jint width, jint height, jstring filepath_) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);
    jsize size = env->GetArrayLength(data_);
    const char *filepath = env->GetStringUTFChars(filepath_, 0);
    LOGI("data frameSize:%d, filepath:%s", size, filepath);

    int ret = saveData(reinterpret_cast<uint8_t *>(data), width, height, filepath);

    // 释放资源
    env->ReleaseByteArrayElements(data_, data, 0);
    env->ReleaseStringUTFChars(filepath_, filepath);
    return 0;
}

/**
 * 协议信息
 */
JNIEXPORT jstring JNICALL
Java_com_vmloft_develop_library_ffmpeg_VMFFmpeg_protocolInfo(JNIEnv *env, jclass type) {

    char info[40000] = {0};
    av_register_all();

    struct URLProtocol *pup = NULL;

    struct URLProtocol **p_temp = (struct URLProtocol **) &pup;
    avio_enum_protocols((void **) p_temp, 0);

    while ((*p_temp) != NULL) {
        sprintf(info, "%sInput: %s\n", info, avio_enum_protocols((void **) p_temp, 0));
    }
    pup = NULL;
    avio_enum_protocols((void **) p_temp, 1);
    while ((*p_temp) != NULL) {
        sprintf(info, "%sInput: %s\n", info, avio_enum_protocols((void **) p_temp, 1));
    }
    LOGI("%s", info);
    return env->NewStringUTF(info);

}
/**
 * 编解码信息
 */
JNIEXPORT jstring JNICALL
Java_com_vmloft_develop_library_ffmpeg_VMFFmpeg_codecInfo(JNIEnv *env, jclass type) {

    char info[40000] = {0};

    av_register_all();

    AVCodec *c_temp = av_codec_next(NULL);

    while (c_temp != NULL) {
        if (c_temp->decode != NULL) {
            sprintf(info, "%sdecode:", info);
        } else {
            sprintf(info, "%sencode:", info);
        }
        switch (c_temp->type) {
        case AVMEDIA_TYPE_VIDEO:
            sprintf(info, "%s(video):", info);
            break;
        case AVMEDIA_TYPE_AUDIO:
            sprintf(info, "%s(audio):", info);
            break;
        case AVMEDIA_TYPE_SUBTITLE:
            sprintf(info, "%s(subtitle):", info);
            break;
        default:
            sprintf(info, "%s(other):", info);
            break;
        }
        sprintf(info, "%s[%10s]\n", info, c_temp->name);
        c_temp = c_temp->next;
    }
    LOGI("%s", info);
    return env->NewStringUTF(info);

}
/**
 * 过滤信息
 */
JNIEXPORT jstring JNICALL
Java_com_vmloft_develop_library_ffmpeg_VMFFmpeg_filterInfo(JNIEnv *env, jclass type) {

    char info[40000] = {0};
    avfilter_register_all();

    AVFilter *f_temp = (AVFilter *) avfilter_next(NULL);
    while (f_temp != NULL) {
        sprintf(info, "%s%s\n", info, f_temp->name);
        f_temp = f_temp->next;
    }
    LOGI("%s", info);
    return env->NewStringUTF(info);

}
/**
 * 格式输入输出消息
 */
JNIEXPORT jstring JNICALL
Java_com_vmloft_develop_library_ffmpeg_VMFFmpeg_formatInfo(JNIEnv *env, jclass type) {

    char info[40000] = {0};

    av_register_all();

    AVInputFormat *if_temp = av_iformat_next(NULL);
    AVOutputFormat *of_temp = av_oformat_next(NULL);
    while (if_temp != NULL) {
        sprintf(info, "%sInput: %s\n", info, if_temp->name);
        if_temp = if_temp->next;
    }
    while (of_temp != NULL) {
        sprintf(info, "%sOutput: %s\n", info, of_temp->name);
        of_temp = of_temp->next;
    }
    LOGI("%s", info);
    return env->NewStringUTF(info);

}
}
