//
// Created by lz liu on 2018/5/15.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/time.h>

#include "VMLog.h"
#include "VMYuvUtil.h"
#include "VMEncoder.h"

enum AVCodecID *avCodecID = AV_CODEC_ID_H264;
AVCodec *avCodec;
AVCodecContext *avCodecContext = NULL;
int frameCount;
AVFrame *avFrame;
AVPacket *avPacket;
FILE *outFile;

uint8_t endcode[] = {0, 0, 1, 0xb7};


int encodeFrame(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt, FILE *outfile) {
    int ret;

    /* send the frame to the encoder */
    if (frame) {
        LOGI("发送帧，开始编码 %5lld", frame->pts);
    }
    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        LOGE("发送编码帧出错");
        return ret;
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return ret;
        } else if (ret < 0) {
            LOGE("编码出错");
            return ret;
        }
        LOGI("写入一帧数据包 frame:%5lld data:size=%5d", pkt->pts, pkt->size);
        fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt);
    }
    return ret;
}

/**
 * 初始化编码器
 * @param outPath
 * @param width
 * @param height
 * @param degrees
 * @return
 */
int initEncoder(const char *outPath, int width, int height, int degrees) {
    int ret = 0;
    // 查找编码器
    avCodec = avcodec_find_encoder(avCodecID);
    if (!avCodec) {
        LOGE("找不到编码器！");
        return -1;
    }

    // 为编码器上下文开辟空间
    avCodecContext = avcodec_alloc_context3(avCodec);
    if (!avCodecContext) {
        LOGE("无法为视频编码器分配内存！");
        return -1;
    }

    // 为数据包分配内存
    avPacket = av_packet_alloc();
    if (!avPacket) {
        LOGE("无法为数据包分配内存！");
        return -1;
    }

    /* put sample parameters */
    avCodecContext->bit_rate = 400000;
    /* resolution must be a multiple of two */
    avCodecContext->width = width / 2;
    avCodecContext->height = height / 2;
    /* frames per second */
    avCodecContext->time_base = (AVRational) {1, 25};
    avCodecContext->framerate = (AVRational) {25, 1};

    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    avCodecContext->gop_size = 25;
    avCodecContext->max_b_frames = 1;
    avCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;

    // 针对编码类型进行不同的设置
    if (avCodec->id == AV_CODEC_ID_H264) {
        av_opt_set(avCodecContext->priv_data, "preset", "slow", 0);
    }
    // 打开编码器
    ret = avcodec_open2(avCodecContext, avCodec, NULL);
    if (ret < 0) {
        LOGE("无法打开编码器: %s", av_err2str(ret));
        return ret;
    }
    // 打开输出文件
    outFile = fopen(outPath, "wb");
    if (!outFile) {
        LOGE("无法打开文件 %s", outPath);
        return ret;
    }

    return ret;
}

/**
 * 编码数据
 * @param data
 * @param width
 * @param height
 * @param degrees
 * @return
 */
int encodeData(uint8_t *data, int width, int height, int degrees) {
    int ret = 0;

    int frameSize = width * height * 3 / 2;
    uint8_t *buffer = av_malloc(frameSize);
    nv21ToI420(data, width, height, buffer);

    // 缩放
    int scaleWidth = width / 2;
    int scaleHeight = height / 2;
    frameSize = scaleWidth * scaleHeight * 3 / 2;
    uint8_t *scaleBuf = av_malloc(frameSize);
    yuvScale(buffer, width, height, scaleBuf, scaleWidth, scaleHeight, 0);

//    // 旋转
//    uint8_t *i420Data = av_malloc(frameSize);
//    yuvRotate(buffer, scaleWidth, scaleHeight, i420Data, degrees);

    avFrame = av_frame_alloc();
    if (!avFrame) {
        LOGE("无法为视频帧分配内存");
        return -1;
    }

    avFrame->format = avCodecContext->pix_fmt;
    avFrame->width = avCodecContext->width;
    avFrame->height = avCodecContext->height;

    ret = av_frame_get_buffer(avFrame, 32);
    if (ret < 0) {
        LOGE("无法分配视频帧数据");
        return ret;
    }

    int ySize = avCodecContext->width * avCodecContext->height;
    avFrame->data[0] = scaleBuf;                  // Y
    avFrame->data[1] = scaleBuf + ySize;          // U
    avFrame->data[2] = scaleBuf + ySize * 5 / 4;  // V

    frameCount++;
    avFrame->pts = frameCount;

    ret = encodeFrame(avCodecContext, avFrame, avPacket, outFile);

    return ret;
}

/**
 * 释放编码器
 */
int freeEncoder() {
    // 刷新编码器，将缓冲区的数据推出编码器
    encodeFrame(avCodecContext, NULL, avPacket, outFile);

    /* add sequence end code to have a real MPEG file */
    fwrite(endcode, 1, sizeof(endcode), outFile);
    fclose(outFile);

    avcodec_free_context(&avCodecContext);
    av_frame_free(&avFrame);
    av_packet_free(&avPacket);
    return 0;
}


/**
 * YUV 数据转图片
 * @param data
 * @param width
 * @param height
 * @param outPath
 * @return
 */
int yuv2Bitmap(uint8_t *data, int width, int height, int degrees, const char *outPath) {
    int64_t startTime = av_gettime();
    int ret = 0;
    int frameSize;

    AVCodec *codec;
    AVCodecContext *codecContext;
    AVFrame *frame;
    AVFormatContext *formatContext;
    AVPacket packet;
    AVStream *stream;

    //    LOGI("开始将 YUV 数据编码保存为图片");
    // 打开文件
    ret = avformat_alloc_output_context2(&formatContext, NULL, NULL, outPath);
    if (ret < 0) {
        LOGE("无法根据文件扩展名获取编码类型，使用默认 MJPEG 编码格式重新初始化");
        ret = avformat_alloc_output_context2(&formatContext, NULL, "mjpeg", outPath);
    }
    if (ret < 0) {
        LOGE("无法打开输出文件 %d, %s", ret, av_err2str(ret));
        return ret;
    }

    // 构建一个新stream
    stream = avformat_new_stream(formatContext, 0);
    if (!stream) {
        LOGE("创建视频流失败");
        return -1;
    }

    // 获取指向视频流编码器上下文的指针
    codecContext = avcodec_alloc_context3(NULL);
    if (codecContext == NULL) {
        printf("无法为编码器上下文指针开辟空间");
        return -1;
    }
    avcodec_parameters_to_context(codecContext, stream->codecpar);

    frameSize = width * height * 3 / 2;
    // 因为 Android 这边摄像头采集到的数据一般是 NV21格式，这里需要将其转为标准的 YUV420P 格式
    uint8_t *buffer = av_malloc(frameSize);
    nv21ToI420(data, width, height, buffer);
    // 旋转
    uint8_t *i420Data = av_malloc(frameSize);
    yuvRotate(buffer, width, height, i420Data, degrees);

    codecContext->codec_id = formatContext->oformat->video_codec;
    codecContext->codec_type = AVMEDIA_TYPE_VIDEO;
    codecContext->pix_fmt = AV_PIX_FMT_YUVJ420P;
    codecContext->width = height;
    codecContext->height = width;
    codecContext->time_base.num = 1;
    codecContext->time_base.den = 25;
    LOGI("视频流编码器设置完成");

    // 输出一些信息
    av_dump_format(formatContext, 0, outPath, 1);

    // 查找解码器
    codec = avcodec_find_encoder(codecContext->codec_id);
    if (!codec) {
        LOGE("找不到编码器");
        return -1;
    }
    // 设置pCodecCtx的解码器为pCodec
    if (avcodec_open2(codecContext, codec, NULL) < 0) {
        LOGE("无法打开编码器");
        return -1;
    }
    LOGI("打开编码器完成 %lld", av_gettime() - startTime);

    frame = av_frame_alloc();

    LOGI("将 NV21 格式数据转为 YUV420P 完成 %lld", av_gettime() - startTime);
    // 将 YUV 数据填充值 frame
    av_image_fill_arrays(frame->data, frame->linesize, i420Data, AV_PIX_FMT_YUVJ420P,
                         codecContext->width, codecContext->height, 1);

    // 写入文件头
    avformat_write_header(formatContext, NULL);
    LOGI("写入文件头信息完成 %lld", av_gettime() - startTime);

    // 给AVPacket分配足够大的空间
    av_new_packet(&packet, frameSize * 2);

    //编码该帧
    avcodec_send_frame(codecContext, frame);
    ret = avcodec_receive_packet(codecContext, &packet);
    if (ret < 0) {
        LOGE("编码失败 %d, %s", ret, av_err2str(ret));
        return ret;
    }
    LOGI("编码完成 %lld", av_gettime() - startTime);
    ret = av_write_frame(formatContext, &packet);
    if (ret < 0) {
        LOGE("写入数据失败 %d, %s", ret, av_err2str(ret));
        return ret;
    }
    LOGI("写入数据完成 %d, %lld", packet.size, av_gettime() - startTime);
    av_packet_unref(&packet);

    // 写入文件尾部信息
    av_write_trailer(formatContext);
    LOGI("写入文件尾信息完成 %lld", av_gettime() - startTime);

    if (stream) {
        avcodec_close(codecContext);
        av_free(frame);
    }
    avio_close(formatContext->pb);
    avformat_free_context(formatContext);
    LOGI("将 YUV 编码保存图片完成 %lld", av_gettime() - startTime);
    return ret;
}

