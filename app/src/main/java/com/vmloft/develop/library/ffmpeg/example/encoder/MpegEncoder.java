package com.vmloft.develop.library.ffmpeg.example.encoder;

import android.util.Log;
import com.vmloft.develop.library.ffmpeg.VMMpegProcess;
import com.vmloft.develop.library.ffmpeg.example.utils.FileManager;
import com.vmloft.develop.library.tools.utils.VMLog;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.LinkedBlockingQueue;

public class MpegEncoder {

    private static MpegEncoder instance;

    private Thread videoEncodeThread;
    private boolean isVideoEncode = false;
    private LinkedBlockingQueue<YUVData> videoQueue;

    private int fps = 0;

    private boolean isTestFile = true;
    private FileManager videoFileManager;

    /**
     * 私有构造
     */
    private MpegEncoder() {
        videoQueue = new LinkedBlockingQueue<>();
        if (isTestFile) {
            videoFileManager = new FileManager(FileManager.TEST_H264_FILE);
        }
    }

    /**
     * 获取单例类实例
     */
    public static MpegEncoder getInstance() {
        if (instance == null) {
            instance = new MpegEncoder();
        }
        return instance;
    }

    /**
     * 开始数据编码
     */
    public void start() {
        startVideoEncode();
    }

    public void stop() {
        stopVideoEncode();
    }

    /**
     * 添加 YUV 数据
     */
    public void putYUVData(YUVData yuvData) {
        if (isVideoEncode && videoQueue != null) {
            try {
                videoQueue.put(yuvData);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    /**
     * 开启视频编码线程
     */
    private void startVideoEncode() {
        if (isVideoEncode) {
            VMLog.e("视频编码已启动");
            return;
        }
        isVideoEncode = true;
        videoEncodeThread = new Thread(new Runnable() {
            @Override
            public void run() {
                while (isVideoEncode && !Thread.interrupted()) {
                    try {
                        // 从队列中取出 YUV 数据
                        YUVData yuvData = videoQueue.take();
                        computeFrameRate();
                        fps++;
                        byte[] outData = new byte[yuvData.width * yuvData.height];
                        int[] outDataLen = new int[10];
                        int numNals = VMMpegProcess.encodeVideoData(yuvData.data, outData.length, fps, outData, outDataLen);
                        if (numNals > 0) {
                            int[] segment = new int[numNals];
                            System.arraycopy(outDataLen, 0, segment, 0, numNals);
                            int totalLength = 0;
                            for (int i = 0; i < numNals; i++) {
                                totalLength += segment[i];
                            }
                            byte[] dstData = new byte[totalLength];
                            System.arraycopy(outData, 0, dstData, 0, totalLength);
                            if (isTestFile) {
                                videoFileManager.saveFileData(dstData);
                            }
                        }
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        });
        videoEncodeThread.start();
    }

    private void stopVideoEncode() {
        isVideoEncode = false;
        if (videoQueue != null) {
            videoQueue.clear();
            videoQueue = null;
        }
        if (videoEncodeThread != null) {
            videoEncodeThread.interrupt();
            videoEncodeThread = null;
        }
    }

    private List<Long> fpsList = new ArrayList<>();

    /**
     * 计算帧率
     */
    public void computeFrameRate() {
        float frameRate;
        fpsList.add(System.currentTimeMillis());
        long time = fpsList.get(fpsList.size() - 1) - fpsList.get(0);
        if (time > 1000) {
            removeFirst();
            time = fpsList.get(fpsList.size() - 1) - fpsList.get(0);
            frameRate = (float) fpsList.size() / time * 1000;
            VMLog.i("编码输出帧率 " + frameRate);
        }
    }

    public void removeFirst() {
        fpsList.remove(0);
        long time = fpsList.get(fpsList.size() - 1) - fpsList.get(0);
        if (time > 1000) {
            removeFirst();
        }
    }
}
