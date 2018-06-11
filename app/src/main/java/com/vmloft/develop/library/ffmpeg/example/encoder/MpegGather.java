package com.vmloft.develop.library.ffmpeg.example.encoder;

import android.hardware.Camera;
import com.vmloft.develop.library.ffmpeg.VMMpegProcess;
import com.vmloft.develop.library.ffmpeg.example.utils.FileManager;
import com.vmloft.develop.library.tools.camera.VMCameraView;
import com.vmloft.develop.library.tools.utils.VMLog;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.LinkedBlockingQueue;

public class MpegGather {

    private static MpegGather instance;
    // 相机预览画面，用来采集视频数据
    private VMCameraView cameraView;
    private VMCameraView.DataListener dataListener;
    // 摄像头采集宽高
    private int cameraWidth;
    private int cameraHeight;
    // 数据最终压缩到的宽高
    private int scaleWidth;
    private int scaleHeight;

    // 存储采集到的摄像头数据队列
    private LinkedBlockingQueue<byte[]> gatherQueue;
    private Thread gatherThread;
    private boolean isRunning = false;

    // 是否测试保存文件
    private boolean isTestFile = true;
    private FileManager fileManager;

    /**
     * 私有构造
     */
    private MpegGather() {
        gatherQueue = new LinkedBlockingQueue<>();
        if (isTestFile) {
            fileManager = new FileManager(FileManager.TEST_YUV_FILE);
        }
    }

    /**
     * 获取单例类实例
     */
    public static MpegGather getInstance() {
        if (instance == null) {
            instance = new MpegGather();
        }
        return instance;
    }

    /**
     * 初始化，需要传递相机预览 View
     *
     * @param view 相机预览 View
     * @param width 相机预览宽
     * @param height 相机预览高
     */
    public void init(VMCameraView view, int width, int height, int sWidth, int sHeight) {
        cameraView = view;
        cameraWidth = width;
        cameraHeight = height;
        scaleWidth = sWidth;
        scaleHeight = sHeight;

        VMMpegProcess.initVideoEncoder(cameraWidth, cameraHeight, scaleWidth, scaleHeight);
    }

    /**
     * 切换相机
     */
    public void switchCamera() {
        cameraView.switchCamera();
    }

    /**
     * 重新开始
     */
    public void resume() {
        cameraView.setCameraWidth(cameraWidth);
        cameraView.setCameraHeight(cameraHeight);
        cameraView.launchCamera();
    }

    /**
     * 开始数据采集
     */
    public void start() {
        setCameraDataListener();
        startGatherData();
    }

    /**
     * 停止数据采集
     */
    public void stop() {
        isRunning = false;
        if (cameraView != null) {
            cameraView.releaseCamera();
        }
        if (gatherQueue != null) {
            gatherQueue.clear();
        }
        if (gatherThread != null) {
            gatherThread.interrupt();
            gatherThread = null;
        }
        if (isTestFile) {
            fileManager.closeFile();
        }
    }

    /**
     * 释放资源
     */
    public void release() {
        isRunning = false;
        if (cameraView != null) {
            cameraView.setDataListener(null);
            cameraView.releaseCamera();
            cameraView = null;
        }
        if (gatherQueue != null) {
            gatherQueue.clear();
            gatherQueue = null;
        }
        if (gatherThread != null) {
            gatherThread.interrupt();
            gatherThread = null;
        }
        VMMpegProcess.release();
    }

    /**
     * 开启采集数据
     */
    private void startGatherData() {
        if (isRunning) {
            VMLog.e("数据采集已经启动");
            return;
        }
        isRunning = true;
        gatherThread = new Thread(new Runnable() {
            @Override
            public void run() {
                while (isRunning && !Thread.interrupted()) {
                    try {
                        byte[] srcData = gatherQueue.take();
                        computeFrameRate();
                        // 摄像头旋转角度
                        int angle = cameraView.getRotateAngle();
                        int width = cameraView.getCameraWidth();
                        int height = cameraView.getCameraHeight();
                        boolean isMirror = cameraView.getCameraId() == Camera.CameraInfo.CAMERA_FACING_FRONT;
                        byte[] dstData = new byte[scaleWidth * scaleHeight * 3 / 2];
                        VMMpegProcess.yuvCompress(srcData, width, height, dstData, scaleHeight, scaleWidth, 0, angle, isMirror);

                        YUVData yuvData = new YUVData(dstData, scaleWidth, scaleHeight);
                        MpegEncoder.getInstance().putYUVData(yuvData);
                        if (isTestFile) {
                            fileManager.saveFileData(dstData);
                        }
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
                VMLog.i("数据采集已结束");
            }
        });
        gatherThread.start();
    }

    /**
     * 设置摄像机数据回调
     */
    private void setCameraDataListener() {
        if (dataListener == null) {
            dataListener = new VMCameraView.DataListener() {
                @Override
                public void onData(byte[] data) {
                    if (data != null && isRunning && gatherQueue != null) {
                        try {
                            gatherQueue.put(data);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                }
            };
        }
        cameraView.setDataListener(dataListener);
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
            VMLog.i("采集输出帧率 " + frameRate);
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
