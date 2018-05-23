package com.vmloft.develop.library.ffmpeg.example;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

import com.vmloft.develop.library.ffmpeg.VMEncoder;
import com.vmloft.develop.library.ffmpeg.VMFFmpeg;
import com.vmloft.develop.library.tools.utils.VMDateUtil;
import com.vmloft.develop.library.tools.utils.VMFileUtil;
import com.vmloft.develop.library.tools.utils.VMLog;
import com.vmloft.develop.library.tools.widget.VMCameraPreview;

/**
 * Created by lzan13 on 2018/5/2.
 * 摄像头采集预览界面
 */
public class CameraActivity extends AppActivity {

    @BindView(R.id.camera_preview_container) LinearLayout previewContainer;
    @BindView(R.id.btn_encode) Button encodeBtn;

    private VMCameraPreview cameraPreview;

    private final int IDLE = 0;
    private final int START = 1;
    private final int RECORD = 2;
    private final int STOPED = 3;

    private int state = IDLE;
    private boolean isTake = false;
    private boolean isSave = false;

    private int width = 1920;
    private int height = 1080;
    private String savePath = VMFileUtil.getDCIM() + "encode.yuv";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ffmpeg);

        ButterKnife.bind(activity);
        init();
    }

    private void init() {
        cameraPreview = VMCameraPreview.newInstance(activity, width, height);
        LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.MATCH_PARENT);
        previewContainer.addView(cameraPreview, layoutParams);

        cameraPreview.setCameraDataCallback(new VMCameraPreview.CameraDataCallback() {
            @Override
            public void onCameraDataCallback(byte[] data, int width, int height, int degrees) {
                //VMLog.d("YUV 数据帧: data:%d, w:%d, h:%d, r:%d", data.length, width, height, degrees);
                if (isSave) {
                    save(data, width, height);
                }
                if (isTake) {
                    isTake = false;
                    takeYuv(data, width, height, degrees);
                }
                if (state != IDLE) {
                    if (state == START) {
                        startRecord(width, height, degrees);
                    } else if (state == RECORD) {
                        encodeData(data, width, height, degrees);
                    } else if (state == STOPED) {
                        stopRecord();
                    }
                }
            }
        });
    }

    @OnClick({ R.id.btn_save, R.id.btn_encode, R.id.btn_filter, R.id.btn_yuv_2_bitmap })
    public void onClick(View view) {
        switch (view.getId()) {
        case R.id.btn_save:
            if (isSave) {
                isSave = false;
            } else {
                isSave = true;
            }
            break;
        case R.id.btn_encode:
            encode();
            break;
        case R.id.btn_filter:
            break;
        case R.id.btn_yuv_2_bitmap:
            isTake = true;
            break;
        }
    }

    private void save(byte[] data, int width, int height) {
        savePath = VMFileUtil.getDCIM() + "I420.yuv";
        VMFFmpeg.saveData(data, width, height, savePath);
    }

    /**
     * 录制保存
     */
    private void encode() {
        if (state == STOPED) {
            return;
        }
        if (state == IDLE) {
            state = START;
            encodeBtn.setText("Stop");
        } else {
            state = STOPED;
            encodeBtn.setText("Encode");
        }
    }

    /**
     * 捕获一帧 YUV 数据，并转为 jpg 格式图片
     */
    private void takeYuv(byte[] data, int width, int height, int degrees) {
        String outPath = VMFileUtil.getDCIM() + "YUV" + VMDateUtil.filenameDateTime() + ".jpg";
        VMEncoder.yuv2Bitmap(data, width, height, degrees, outPath);
    }

    /**
     * 开始录制，主要是进行编码器的初始化操作
     */
    private void startRecord(int width, int height, int degrees) {
        savePath = VMFileUtil.getDCIM() + "encode" + "." + "flv";
        int result = VMEncoder.initEncoder(savePath, width, height, degrees);
        if (result >= 0) {
            state = RECORD;
        }
    }

    /**
     * 编码数据
     */
    private void encodeData(byte[] data, int width, int height, int degrees) {
        VMEncoder.encodeData(data, width, height, degrees);
    }

    /**
     * 停止录制
     */
    private void stopRecord() {
        VMEncoder.freeEncoder();
        state = IDLE;
    }
}
