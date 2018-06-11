package com.vmloft.develop.library.ffmpeg;

public class VMMpegProcess {

    /**
     * 加载底层库
     */
    static {
        System.loadLibrary("vmmpeg");
    }

    /**
     * ------------------------- 视频编码相关 -------------------------
     */
    /**
     * 初始化视频编码器
     *
     * @param inWidth 输入数据宽度
     * @param inHeight 输入数据高度
     * @param outWidth 输出数据宽度
     * @param outHeight 输出数据高度
     */
    public static native boolean initVideoEncoder(int inWidth, int inHeight, int outWidth, int outHeight);

    /**
     * 编码视频数据
     *
     * @param srcData 数据源
     * @param frameSize 数据源数据大小
     * @param fps 帧
     * @param dstData 目标数据
     * @param outFrameSize 目标数据大小
     */
    public static native int encodeVideoData(byte[] srcData, int frameSize, int fps, byte[] dstData, int[] outFrameSize);

    /**
     * 处理器的释放
     */
    public static native int release();

    /**
     * ------------------------- 数据处理相关 -------------------------
     */
    /**
     * NV21 转 I420
     *
     * @param srcNV21 源数据为 NV21 格式
     * @param width 源数据宽
     * @param height 源数据高
     * @param dstData 目标数据 I420 格式
     */
    public static native int nv21ToI420(byte[] srcNV21, int width, int height, byte[] dstData);

    /**
     * 缩放 YUV 数据，这里需要标准的 YUV420P 格式 YUV 数据 如果是 NV21 格式可以调用 nv21ToI420() 进行转换
     *
     * @param srcData 源数据为 I420 格式
     * @param width 源宽高
     * @param height
     * @param dstData 目标数据
     * @param dstWidth 目标宽高
     * @param dstHeight
     * @param mode 缩放模式，0 为最快，3 为质量最好
     */
    public static native int yuvScale(byte[] srcData, int width, int height, byte[] dstData, int dstWidth, int dstHeight, int mode);

    /**
     * 旋转 YUV 数据，Android 摄像头一般都会旋转 90 或 270 度
     *
     * @param srcData 源数据为 I420 格式
     * @param width 源宽高
     * @param height
     * @param dstData 目标数据
     * @param angle 旋转角度
     */
    public static native int yuvRotate(byte[] srcData, int width, int height, byte[] dstData, int angle);

    /**
     * 镜像 YUV 数据，一般前置摄像头会镜像数据，如果想保存的数据正常就需要进行镜像操作
     *
     * @param srcData 源数据为 I420 格式
     * @param width 源宽高
     * @param height
     * @param dstData 目标数据
     */
    public static native int yuvMirror(byte[] srcData, int width, int height, byte[] dstData);

    /**
     * 剪切 YUV 数据
     *
     * @param srcData 源数据为 I420 格式
     * @param width 源宽高
     * @param height
     * @param dstData 目标数据
     * @param dstWidth 目标宽高
     * @param dstHeight
     * @param left 剪切起点坐标
     * @param top
     */
    public static native int yuvCrop(byte[] srcData, int width, int height, byte[] dstData, int dstWidth, int dstHeight, int left, int top);

    /**
     * 压缩 YUV 数据，这中间会包括 转换、旋转、缩放、镜像等
     *
     * @param srcData 源数据
     * @param width 输入宽高
     * @param height
     * @param dstData 目标数据
     * @param dstWidth 目标宽高
     * @param dstHeight
     * @param mode 压缩模式 0 最快 3 质量最好
     * @param angle 旋转角度
     * @param isMirror 是否镜像
     */
    public static native int yuvCompress(byte[] srcData, int width, int height, byte[] dstData, int dstWidth, int dstHeight, int mode, int angle, boolean isMirror);

    /**
     * ------------------------- 推流相关 -------------------------
     */
    /**
     * 初始化推流相关操作，建立与 RTMP 服务器链接¬
     *
     * @param url 推流地址
     */
    public static native int initPublish(String url);

    /**
     * 发送视频数据头信息
     *
     * @param sps sps 数据
     * @param spsLen sps 数据长度
     * @param pps pps 数据
     * @param ppsLen pps 数据长度
     * @param timestamp 时间戳
     */
    public static native int sendVideoHeader(byte[] sps, int spsLen, byte[] pps, int ppsLen, long timestamp);

    /**
     * 发送视频数据
     *
     * @param data 视频数据
     * @param dataLen 视频数据长度
     * @param timestamp 时间戳
     */
    public static native int sendVideoData(byte[] data, int dataLen, long timestamp);

    /**
     * 释放推流链接
     */
    public static native int releasePublish();
}
