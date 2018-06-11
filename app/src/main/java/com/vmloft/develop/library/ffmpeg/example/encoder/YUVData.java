package com.vmloft.develop.library.ffmpeg.example.encoder;

public class YUVData {

    public byte[] data;
    public int width;
    public int height;

    public YUVData(byte[] data, int width, int height) {
        this.data = data;
        this.width = width;
        this.height = height;
    }
}
