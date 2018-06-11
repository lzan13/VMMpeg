//
// Created by lz liu on 2018/6/7.
//

#include "libyuv.h"

using namespace libyuv;

#include "VMLog.h"
#include "VMYuv.h"

uint8_t *temp_i420_data;
uint8_t *temp_i420_data_scale;
uint8_t *temp_i420_data_rotate;

/**
 * 初始化 YUV 数据转换
 * @param width 数据源宽高
 * @param height
 * @param dstWidth 输出数据宽高
 * @param dstHeight
 */
void yuvInit(int width, int height, int dstWidth, int dstHeight) {
    // 为存储临时数据分配内存空间
    temp_i420_data = (uint8_t *) malloc(sizeof(uint8_t) * width * height * 3 / 2);
    temp_i420_data_scale = (uint8_t *) malloc(sizeof(uint8_t) * dstWidth * dstHeight * 3 / 2);
    temp_i420_data_rotate = (uint8_t *) malloc(sizeof(uint8_t) * dstWidth * dstHeight * 3 / 2);
}

void release() {
    free(temp_i420_data);
    free(temp_i420_data_scale);
    free(temp_i420_data_rotate);
}

/**
 * NV21 转 I420
 * @param srcNV21 源数据为 NV21 格式
 * @param width 源宽高
 * @param height
 * @param dstData 目标数据 I420 格式
 */
void nv21ToI420(uint8_t *srcNV21, int width, int height, uint8_t *dstData) {
    //    LOGI("NV21 转 I420 开始 w:%d, :%d", width, height);
    int src_y_size = width * height;
    int src_u_size = (width >> 1) * (height >> 1);

    uint8_t *src_nv21_y_data = srcNV21;
    uint8_t *src_nv21_vu_data = srcNV21 + src_y_size;

    uint8_t *src_i420_y_data = dstData;
    uint8_t *src_i420_u_data = dstData + src_y_size;
    uint8_t *src_i420_v_data = dstData + src_y_size + src_u_size;


    NV21ToI420(src_nv21_y_data, width, src_nv21_vu_data, width, src_i420_y_data, width,
               src_i420_u_data, width >> 1, src_i420_v_data, width >> 1, width, height);
    //    LOGI("NV21 转 I420 完成");
}

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
void yuvScale(uint8_t *srcData, int width, int height, uint8_t *dstData, int dstWidth, int dstHeight, int mode) {
    //    LOGI("缩放数据开始 w:%d->%d, :%d->%d", width, dstWidth, height, dstHeight);

    int src_i420_y_size = width * height;
    int src_i420_u_size = (width >> 1) * (height >> 1);
    uint8_t *src_i420_y_data = srcData;
    uint8_t *src_i420_u_data = srcData + src_i420_y_size;
    uint8_t *src_i420_v_data = srcData + src_i420_y_size + src_i420_u_size;

    int dst_i420_y_size = dstWidth * dstHeight;
    int dst_i420_u_size = (dstWidth >> 1) * (dstHeight >> 1);
    uint8_t *dst_i420_y_data = dstData;
    uint8_t *dst_i420_u_data = dstData + dst_i420_y_size;
    uint8_t *dst_i420_v_data = dstData + dst_i420_y_size + dst_i420_u_size;

    I420Scale(src_i420_y_data, width, src_i420_u_data, width >> 1, src_i420_v_data, width >> 1,
              width, height, dst_i420_y_data, dstWidth, dst_i420_u_data, dstWidth >> 1,
              dst_i420_v_data, dstWidth >> 1, dstWidth, dstHeight, (enum FilterMode) mode);
    //    LOGI("缩放数据完成");
}

/**
 * 旋转 YUV 数据，Android 摄像头一般都会旋转 90 或 270 度
 * @param srcData 源数据为 I420 格式
 * @param width 源宽高
 * @param height
 * @param dstData 目标数据
 * @param angle 旋转角度
 */
void yuvRotate(uint8_t *srcData, int width, int height, uint8_t *dstData, int angle) {
    //    LOGI("旋转数据开始 angle:%d, w:%d, :%d", angle, width, height);
    int src_i420_y_size = width * height;
    int src_i420_u_size = (width >> 1) * (height >> 1);

    uint8_t *src_i420_y_data = srcData;
    uint8_t *src_i420_u_data = srcData + src_i420_y_size;
    uint8_t *src_i420_v_data = srcData + src_i420_y_size + src_i420_u_size;

    uint8_t *dst_i420_y_data = dstData;
    uint8_t *dst_i420_u_data = dstData + src_i420_y_size;
    uint8_t *dst_i420_v_data = dstData + src_i420_y_size + src_i420_u_size;

    //要注意这里的width和height在旋转之后是相反的
    if (angle == kRotate90 || angle == kRotate270) {
        I420Rotate(src_i420_y_data, width, src_i420_u_data, width >> 1, src_i420_v_data, width >> 1,
                   dst_i420_y_data, height, dst_i420_u_data, height >> 1, dst_i420_v_data,
                   height >> 1, width, height, (enum RotationMode) angle);
    }
    //    LOGI("旋转数据完成");
}

/**
 * 镜像 YUV 数据，一般前置摄像头会镜像数据，如果想保存的数据正常就需要进行镜像操作
 * @param srcData 源数据为 I420 格式
 * @param width 源宽高
 * @param height
 * @param dstData 目标数据
 */
void yuvMirror(uint8_t *srcData, int width, int height, uint8_t *dstData) {
    //    LOGI("镜像数据开始 w:%d, :%d", width, height);
    int src_i420_y_size = width * height;
    int src_i420_u_size = (width >> 1) * (height >> 1);

    uint8_t *src_i420_y_data = srcData;
    uint8_t *src_i420_u_data = srcData + src_i420_y_size;
    uint8_t *src_i420_v_data = srcData + src_i420_y_size + src_i420_u_size;

    uint8_t *dst_i420_y_data = dstData;
    uint8_t *dst_i420_u_data = dstData + src_i420_y_size;
    uint8_t *dst_i420_v_data = dstData + src_i420_y_size + src_i420_u_size;

    I420Mirror(src_i420_y_data, width, src_i420_u_data, width >> 1, src_i420_v_data, width >> 1,
               dst_i420_y_data, width, dst_i420_u_data, width >> 1, dst_i420_v_data, width >> 1,
               width, height);
    //    LOGI("镜像数据完成");
}

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
void yuvCrop(uint8_t *srcData, int width, int height, uint8_t *dstData, int dstWidth, int dstHeight, int left, int top) {
    //    LOGI("剪切数据开始 w:%d->%d, :%d->%d", width, dstWidth, height, dstHeight);
    //裁剪的区域大小不对
    if (left + dstWidth > width || top + dstHeight > height) {
        LOGE("剪切区域大小有问题");
        return;
    }

    //left和top必须为偶数，否则显示会有问题
    if (left % 2 != 0 || top % 2 != 0) {
        LOGE("剪切起点必须是偶数");
        return;
    }

    int src_length = width * height * 3 / 2;

    int dst_i420_y_size = dstWidth * dstHeight;
    int dst_i420_u_size = (dstWidth >> 1) * (dstHeight >> 1);

    uint8_t *dst_i420_y_data = dstData;
    uint8_t *dst_i420_u_data = dstData + dst_i420_y_size;
    uint8_t *dst_i420_v_data = dstData + dst_i420_y_size + dst_i420_u_size;

    ConvertToI420((const uint8 *) srcData, src_length, dst_i420_y_data, dstWidth, dst_i420_u_data,
                  dstWidth >> 1, dst_i420_v_data, dstWidth >> 1, left, top, width, height, dstWidth,
                  dstHeight, kRotate0, FOURCC_I420);
    //    LOGI("剪切数据完成");
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
void yuvCompress(uint8_t *srcData, int width, int height, uint8_t *dstData, int dstWidth, int dstHeight, int mode, int angle, bool isMirror) {
    LOGI("压缩 YUV 数据开始，其中包含 NV21 转 I420，缩放，旋转，镜像");
    // nv21 转化为 i420 (标准YUV420P数据) 这个 temp_i420_data 大小是和 srcData 是一样的
    nv21ToI420(srcData, width, height, temp_i420_data);
    // 进行缩放的操作，这个缩放，会把数据压缩
    yuvScale(temp_i420_data, width, height, temp_i420_data_scale, dstWidth, dstHeight, mode);
    // 如果是前置摄像头，进行镜像操作
    if (isMirror) {
        // 进行旋转的操作
        yuvRotate(temp_i420_data_scale, dstWidth, dstHeight, temp_i420_data_rotate, angle);
        // 因为旋转的角度都是90和270，那后面的数据width和height是相反的
        yuvMirror(temp_i420_data_rotate, dstHeight, dstWidth, dstData);
    } else {
        // 进行旋转的操作
        yuvRotate(temp_i420_data_scale, dstWidth, dstHeight, dstData, angle);
    }
    LOGI("压缩 YUV 数据完成");
}
