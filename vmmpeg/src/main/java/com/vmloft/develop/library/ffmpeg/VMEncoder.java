package com.vmloft.develop.library.ffmpeg;

/**
 * Created by lzan13 on 2018/5/11.
 */
public class VMEncoder {

    public static native int initEncoder(String outPath, int width, int height, int degrees);

    public static native int encodeData(byte[] data, int widht, int height, int degrees);

    public static native int freeEncoder();

    public static native int yuv2Bitmap(byte[] data, int width, int height, int degrees, String outPath);
}
