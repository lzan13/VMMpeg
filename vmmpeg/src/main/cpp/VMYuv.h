//
// Created by lz liu on 2018/6/7.
//

#ifndef VMMPEG_VMYUV_H
#define VMMPEG_VMYUV_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 初始化 YUV 数据转换
 * @param width 数据源宽高
 * @param height
 * @param dstWidth 输出数据宽高
 * @param dstHeight
 */
void yuvInit(int width, int height, int dstWidth, int dstHeight);

/**
 * 释放分配的内存空间
 */
void release();

/**
 * NV21 转 I420
 * @param srcNV21 源数据为 NV21 格式
 * @param width 源宽高
 * @param height
 * @param dstData 目标数据 I420 格式
 */
void nv21ToI420(uint8_t *srcNV21, int width, int height, uint8_t *dstData);

/**
 * 缩放 YUV 数据，这里需要标准的 YUV420P 格式 YUV 数据 如果是 NV21 格式可以调用 nv21ToI420() 进行转换
 * @param srcData 源数据为 I420 格式
 * @param width 源宽高
 * @param height
 * @param dstData 目标数据
 * @param dstWidth 目标宽高
 * @param dstHeight
 * @param mode 缩放模式，0 为最快，3 为质量最好
 */
void yuvScale(uint8_t *srcData, int width, int height, uint8_t *dstData, int dstWidth, int dstHeight, int mode);

/**
 * 旋转 YUV 数据，Android 摄像头一般都会旋转 90 或 270 度
 * @param srcData 源数据为 I420 格式
 * @param width 源宽高
 * @param height
 * @param dstData 目标数据
 * @param angle 旋转角度
 */
void yuvRotate(uint8_t *srcData, int width, int height, uint8_t *dstData, int angle);

/**
 * 镜像 YUV 数据，一般前置摄像头会镜像数据，如果想保存的数据正常就需要进行镜像操作
 * @param srcData 源数据为 I420 格式
 * @param width 源宽高
 * @param height
 * @param dstData 目标数据
 */
void yuvMirror(uint8_t *srcData, int width, int height, uint8_t *dstData);

/**
 * 剪切 YUV 数据
 * @param srcData 源数据为 I420 格式
 * @param width 源宽高
 * @param height
 * @param dstData 目标数据
 * @param dstWidth 目标宽高
 * @param dstHeight
 * @param left 剪切起点坐标
 * @param top
 */
void yuvCrop(uint8_t *srcData, int width, int height, uint8_t *dstData, int dstWidth, int dstHeight, int left, int top);

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
void yuvCompress(uint8_t *srcData, int width, int height, uint8_t *dstData, int dstWidth, int dstHeight, int mode, int angle, bool isMirror);

#ifdef __cplusplus
};
#endif
#endif //VMMPEG_VMYUV_H
