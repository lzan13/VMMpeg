//
// Created by lz liu on 2018/5/23.
//

#ifndef VMFFMPEG_VMYUVUTIL_H
#define VMFFMPEG_VMYUVUTIL_H

#include <stdint.h>

/**
 * 缩放 YUV 数据，这里需要标准的 YUV420P 格式 YUV 数据 如果是 NV21 格式可以调用 nv21ToI420() 进行转换
 * @param srcI420 源数据为 I420 格式
 * @param width 源宽高
 * @param height
 * @param dstI420 目标数据
 * @param dstWidth 目标宽高
 * @param dstHeight
 * @param mode 缩放模式，0 为最快，3 为质量最好
 */
void yuvScale(uint8_t *srcI420, int width, int height, uint8_t *dstI420, int dstWidth, int dstHeight, int mode);

/**
 * 旋转 YUV 数据，Android 摄像头一般都会旋转 90 或 270 度
 * @param srcI420 源数据为 I420 格式
 * @param width 源宽高
 * @param height
 * @param dstI420 目标数据
 * @param degree 旋转角度
 */
void yuvRotate(uint8_t *srcI420, int width, int height, uint8_t *dstI420, int degree);

/**
 * 镜像 YUV 数据，一般前置摄像头会镜像数据，如果想保存的数据正常就需要进行镜像操作
 * @param srcI420 源数据为 I420 格式
 * @param width 源宽高
 * @param height
 * @param dstI420 目标数据
 */
void yuvMirror(uint8_t *srcI420, int width, int height, uint8_t *dstI420);

/**
 * NV21 转 I420
 * @param srcNV21 源数据为 NV21 格式
 * @param width 源宽高
 * @param height
 * @param dstI420 目标数据 I420 格式
 */
void nv21ToI420(uint8_t *srcNV21, int width, int height, uint8_t *dstI420);

/**
 * 剪切 YUV 数据
 * @param srcI420 源数据为 I420 格式
 * @param width 源宽高
 * @param height
 * @param dstI420 目标数据
 * @param dstWidth 目标宽高
 * @param dstHeight
 * @param left 剪切起点坐标
 * @param top
 */
void yuvCrop(uint8_t *srcI420, int width, int height, uint8_t *dstI420, int dstWidth, int dstHeight, int left, int top);

#endif //VMFFMPEG_VMYUVUTIL_H
