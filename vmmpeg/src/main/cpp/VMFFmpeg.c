//
// Created by lz liu on 2018/5/18.
//

#include <stdio.h>
#include <string.h>
#include <libavutil/mem.h>

#include "VMLog.h"
#include "VMYuv.h"
#include "VMFFmpeg.h"


/**
 * 保存 YUV 数据到文件，这里是直接以二进制方式写入
 * @param data YUV 数据
 * @param width YUV 图像宽
 * @param height YUV 图像高
 * @param filepath 输出文件路径
 */
int saveData(uint8_t *data, int width, int height, const char *filepath) {
    int size = width * height * 3 / 2;
    LOGI("data size:%d, filepath:%s", size, filepath);

    uint8_t *buffer = av_malloc(size);
    nv21ToI420(data, width, height, buffer);

    uint8_t *i420Data = av_malloc(size);
    yuvRotate(buffer, width, height, i420Data, 90);

    // wb+ 以可读写方式打开或创建一个二进制文件
    FILE *outFile = fopen(filepath, "awb+");
    if (!outFile) {
        LOGE("无法打开输出文件");
        return -1;
    }
    // 将数据写入文件
    int ret = fwrite(i420Data, sizeof(uint8_t), size, outFile);
    // 关闭文件
    fclose(outFile);
    LOGE("数据保存完成");
    return ret;
}

