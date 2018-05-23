//
// Created by lz liu on 2018/5/18.
//
#include <stdint.h>

#ifndef VMFFMPEG_VMFFMPEG_H
#define VMFFMPEG_VMFFMPEG_H

/**
 * 保存 YUV 数据到文件，这里是直接以二进制方式写入
 * @param data YUV 数据
 * @param width YUV 图像宽
 * @param height YUV 图像高
 * @param filepath 输出文件路径
 */
int saveData(uint8_t *data, int width, int height, const char *savePath);

#endif //VMFFMPEG_VMFFMPEG_H
