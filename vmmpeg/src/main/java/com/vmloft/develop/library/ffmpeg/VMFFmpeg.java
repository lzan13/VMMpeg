package com.vmloft.develop.library.ffmpeg;

/**
 * Created by lzan13 on 2018/5/2.
 */
public class VMFFmpeg {

    /**
     * 保存数据
     *
     * @param data 数据
     * @param filepath 文件路径
     */
    public static native int saveData(byte[] data, int width, int height, String filepath);

    public static native String protocolInfo();

    public static native String codecInfo();

    public static native String filterInfo();

    public static native String formatInfo();


}
