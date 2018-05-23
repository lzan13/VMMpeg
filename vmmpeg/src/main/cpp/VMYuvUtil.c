//
// Created by lz liu on 2018/5/23.
//

#include "libyuv.h"

#include "VMLog.h"
#include "VMYuvUtil.h"

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
void yuvScale(uint8_t *srcI420, int width, int height, uint8_t *dstI420, int dstWidth, int dstHeight, int mode) {
    LOGI("I420 缩放 开始");

    int src_i420_y_size = width * height;
    int src_i420_u_size = (width >> 1) * (height >> 1);
    uint8_t *src_i420_y_data = srcI420;
    uint8_t *src_i420_u_data = srcI420 + src_i420_y_size;
    uint8_t *src_i420_v_data = srcI420 + src_i420_y_size + src_i420_u_size;

    int dst_i420_y_size = dstWidth * dstHeight;
    int dst_i420_u_size = (dstWidth >> 1) * (dstHeight >> 1);
    uint8_t *dst_i420_y_data = dstI420;
    uint8_t *dst_i420_u_data = dstI420 + dst_i420_y_size;
    uint8_t *dst_i420_v_data = dstI420 + dst_i420_y_size + dst_i420_u_size;

    I420Scale(src_i420_y_data, width, src_i420_u_data, width >> 1, src_i420_v_data, width >> 1,
              width, height, dst_i420_y_data, dstWidth, dst_i420_u_data, dstWidth >> 1,
              dst_i420_v_data, dstWidth >> 1, dstWidth, dstHeight, (enum FilterMode) mode);
    LOGI("I420 缩放 完成");
}

/**
 * 旋转 YUV 数据，Android 摄像头一般都会旋转 90 或 270 度
 * @param srcI420 源数据为 I420 格式
 * @param width 源宽高
 * @param height
 * @param dstI420 目标数据
 * @param degree 旋转角度
 */
void yuvRotate(uint8_t *srcI420, int width, int height, uint8_t *dstI420, int degree) {
    LOGI("I420 旋转 %d 开始", degree);
    int src_i420_y_size = width * height;
    int src_i420_u_size = (width >> 1) * (height >> 1);

    uint8_t *src_i420_y_data = srcI420;
    uint8_t *src_i420_u_data = srcI420 + src_i420_y_size;
    uint8_t *src_i420_v_data = srcI420 + src_i420_y_size + src_i420_u_size;

    uint8_t *dst_i420_y_data = dstI420;
    uint8_t *dst_i420_u_data = dstI420 + src_i420_y_size;
    uint8_t *dst_i420_v_data = dstI420 + src_i420_y_size + src_i420_u_size;

    //要注意这里的width和height在旋转之后是相反的
    if (degree == kRotate90 || degree == kRotate270) {
        I420Rotate(src_i420_y_data, width, src_i420_u_data, width >> 1, src_i420_v_data, width >> 1,
                   dst_i420_y_data, height, dst_i420_u_data, height >> 1, dst_i420_v_data,
                   height >> 1, width, height, (enum RotationMode) degree);
    }
    LOGI("I420 旋转 %d 完成", degree);
}

/**
 * 镜像 YUV 数据，一般前置摄像头会镜像数据，如果想保存的数据正常就需要进行镜像操作
 * @param srcI420 源数据为 I420 格式
 * @param width 源宽高
 * @param height
 * @param dstI420 目标数据
 */
void yuvMirror(uint8_t *srcI420, int width, int height, uint8_t *dstI420) {
    LOGI("I420 镜像 开始");
    int src_i420_y_size = width * height;
    int src_i420_u_size = (width >> 1) * (height >> 1);

    uint8_t *src_i420_y_data = srcI420;
    uint8_t *src_i420_u_data = srcI420 + src_i420_y_size;
    uint8_t *src_i420_v_data = srcI420 + src_i420_y_size + src_i420_u_size;

    uint8_t *dst_i420_y_data = dstI420;
    uint8_t *dst_i420_u_data = dstI420 + src_i420_y_size;
    uint8_t *dst_i420_v_data = dstI420 + src_i420_y_size + src_i420_u_size;

    I420Mirror(src_i420_y_data, width, src_i420_u_data, width >> 1, src_i420_v_data, width >> 1,
               dst_i420_y_data, width, dst_i420_u_data, width >> 1, dst_i420_v_data, width >> 1,
               width, height);
    LOGI("I420 镜像 完成");
}


/**
 * NV21 转 I420
 * @param srcNV21 源数据为 NV21 格式
 * @param width 源宽高
 * @param height
 * @param dstI420 目标数据 I420 格式
 */
void nv21ToI420(uint8_t *srcNV21, int width, int height, uint8_t *dstI420) {
    LOGI("NV21 转为 I420 开始");
    int src_y_size = width * height;
    int src_u_size = (width >> 1) * (height >> 1);

    uint8_t *src_nv21_y_data = srcNV21;
    uint8_t *src_nv21_vu_data = srcNV21 + src_y_size;

    uint8_t *src_i420_y_data = dstI420;
    uint8_t *src_i420_u_data = dstI420 + src_y_size;
    uint8_t *src_i420_v_data = dstI420 + src_y_size + src_u_size;


    NV21ToI420(src_nv21_y_data, width, src_nv21_vu_data, width, src_i420_y_data, width,
               src_i420_u_data, width >> 1, src_i420_v_data, width >> 1, width, height);
    LOGI("NV21 转 I420 完成");
}

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
void yuvCrop(uint8_t *srcI420, int width, int height, uint8_t *dstI420, int dstWidth, int dstHeight, int left, int top) {
    //裁剪的区域大小不对
    if (left + dstWidth > width || top + dstHeight > height) {
        return;
    }

    //left和top必须为偶数，否则显示会有问题
    if (left % 2 != 0 || top % 2 != 0) {
        return;
    }

    int src_length = width * height * 3 / 2;

    int dst_i420_y_size = dstWidth * dstHeight;
    int dst_i420_u_size = (dstWidth >> 1) * (dstHeight >> 1);

    uint8_t *dst_i420_y_data = dstI420;
    uint8_t *dst_i420_u_data = dstI420 + dst_i420_y_size;
    uint8_t *dst_i420_v_data = dstI420 + dst_i420_y_size + dst_i420_u_size;

    ConvertToI420((const uint8 *) srcI420, src_length, dst_i420_y_data, dstWidth, dst_i420_u_data,
                  dstWidth >> 1, dst_i420_v_data, dstWidth >> 1, left, top, width, height, dstWidth,
                  dstHeight, kRotate0, FOURCC_I420);

}
