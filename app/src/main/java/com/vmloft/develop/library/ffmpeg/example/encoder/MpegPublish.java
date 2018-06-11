package com.vmloft.develop.library.ffmpeg.example.encoder;

import android.util.Log;
import com.vmloft.develop.library.ffmpeg.VMMpegProcess;
import com.vmloft.develop.library.tools.utils.VMLog;
import java.util.concurrent.LinkedBlockingQueue;

public class MpegPublish {

    public static final int NAL_UNKNOWN = 0;
    public static final int NAL_SLICE = 1; /* 非关键帧 */
    public static final int NAL_SLICE_DPA = 2;
    public static final int NAL_SLICE_DPB = 3;
    public static final int NAL_SLICE_DPC = 4;
    public static final int NAL_SLICE_IDR = 5; /* 关键帧 */
    public static final int NAL_SEI = 6;
    public static final int NAL_SPS = 7; /* SPS帧 */
    public static final int NAL_PPS = 8; /* PPS帧 */
    public static final int NAL_AUD = 9;
    public static final int NAL_FILLER = 12;

    private static MpegPublish instance;
    private String serverUrl;

    private boolean isPublish;
    private Thread publishThread;
    private LinkedBlockingQueue<Runnable> publishQueue;

    private long videoID = 0;

    private MpegPublish() {}

    public static MpegPublish getInstance() {
        if (instance == null) {
            instance = new MpegPublish();
        }
        return instance;
    }

    public void init(final String url) {
        serverUrl = url;
        publishQueue = new LinkedBlockingQueue<>();

        connectPublishServer(url);

        startPublish();
    }

    /**
     * 开始推流
     */
    public void startPublish() {
        if (isPublish) {
            VMLog.e("推流已经开始");
            return;
        }
        isPublish = true;
        publishThread = new Thread(new Runnable() {
            @Override
            public void run() {
                while (isPublish && !Thread.interrupted()) {
                    try {
                        Runnable runnable = publishQueue.take();
                        runnable.run();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        });
        publishThread.start();
    }

    public void publishData(byte[] data, int dataLen, int[] segment) {
        if (isPublish) {
            onEncoderVideoData(data, dataLen, segment);
        }
    }

    /**
     * 处理推送视频
     *
     * @param data
     * @param dataLen
     * @param segment
     */
    private void onEncoderVideoData(byte[] data, int dataLen, int[] segment) {
        int spsLen = 0;
        int ppsLen = 0;
        byte[] sps = null;
        byte[] pps = null;
        int haveCopy = 0;
        //segment为C++传递上来的数组，当为SPS，PPS的时候，视频NALU数组大于1，其它时候等于1
        for (int i = 0; i < segment.length; i++) {
            int segmentLength = segment[i];
            byte[] segmentByte = new byte[segmentLength];
            System.arraycopy(data, haveCopy, segmentByte, 0, segmentLength);
            haveCopy += segmentLength;

            int offset = 4;
            if (segmentByte[2] == 0x01) {
                offset = 3;
            }
            int type = segmentByte[offset] & 0x1f;
            //Log.d("RiemannLee", "type= " + type);
            //获取到NALU的type，SPS，PPS，SEI，还是关键帧
            if (type == NAL_SPS) {
                spsLen = segment[i] - 4;
                sps = new byte[spsLen];
                System.arraycopy(segmentByte, 4, sps, 0, spsLen);
                //Log.e("RiemannLee", "NAL_SPS spsLen " + spsLen);
            } else if (type == NAL_PPS) {
                ppsLen = segment[i] - 4;
                pps = new byte[ppsLen];
                System.arraycopy(segmentByte, 4, pps, 0, ppsLen);
                //Log.e("RiemannLee", "NAL_PPS ppsLen " + ppsLen);
                sendVideoSpsAndPPS(sps, spsLen, pps, ppsLen, 0);
            } else {
                //如果是关键帧，则在发送该帧之前先发送SPS和PPS
                sendVideoData(segmentByte, segmentLength, videoID++);
            }
        }
    }

    /**
     * 发送视频数据头
     *
     * @param sps
     * @param spsLen
     * @param pps
     * @param ppsLen
     * @param timeStamp
     */
    private void sendVideoSpsAndPPS(final byte[] sps, final int spsLen, final byte[] pps, final int ppsLen, final long timeStamp) {
        Runnable runnable = new Runnable() {
            @Override
            public void run() {
                VMMpegProcess.sendVideoHeader(sps, spsLen, pps, ppsLen, timeStamp);
            }
        };
        try {
            publishQueue.put(runnable);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    /**
     * 发送视频数据
     *
     * @param data
     * @param dataLen
     * @param timeStamp
     */
    private void sendVideoData(final byte[] data, final int dataLen, final long timeStamp) {
        Runnable runnable = new Runnable() {
            @Override
            public void run() {
                VMMpegProcess.sendVideoData(data, dataLen, timeStamp);
            }
        };
        try {
            publishQueue.put(runnable);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    /**
     * 链接推流服务器
     */
    private void connectPublishServer(final String url) {
        Runnable runnable = new Runnable() {
            @Override
            public void run() {
                VMMpegProcess.initPublish(url);
            }
        };
        try {
            publishQueue.put(runnable);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    public void release() {
        Runnable runnable = new Runnable() {
            @Override
            public void run() {
                VMMpegProcess.releasePublish();
            }
        };
    }
}
