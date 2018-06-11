package com.vmloft.develop.library.ffmpeg.example;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

import com.vmloft.develop.library.ffmpeg.example.encoder.MpegEncoder;
import com.vmloft.develop.library.ffmpeg.example.encoder.MpegGather;
import com.vmloft.develop.library.tools.camera.VMCameraView;

/**
 * Created by lzan13 on 2018/5/2.
 * 摄像头采集预览界面
 */
public class CameraActivity extends AppActivity {

    @BindView(R.id.widget_camera_view) VMCameraView cameraView;
    @BindView(R.id.btn_encode) Button encodeBtn;

    private boolean isEncode = false;
    private int width = 1920;
    private int height = 1080;
    private int scaleWidth = 480;
    private int scaleHeight = 640;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera);

        ButterKnife.bind(activity);

        init();
    }

    private void init() {
        MpegGather.getInstance().init(cameraView, width, height, scaleWidth, scaleHeight);
    }

    @OnClick({ R.id.btn_encode })
    public void onClick(View view) {
        switch (view.getId()) {
        case R.id.btn_encode:
            encode();
            break;
        }
    }

    /**
     * 编码
     */
    private void encode() {
        if (isEncode) {
            isEncode = false;
            MpegGather.getInstance().stop();
            MpegEncoder.getInstance().stop();
            encodeBtn.setText("Encode");
        } else {
            isEncode = true;
            MpegGather.getInstance().start();
            MpegEncoder.getInstance().start();
            encodeBtn.setText("Stop");
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        MpegGather.getInstance().resume();
    }

    @Override
    protected void onStop() {
        super.onStop();
        MpegGather.getInstance().stop();
        MpegEncoder.getInstance().stop();
    }

    @Override
    protected void onDestroy() {
        MpegGather.getInstance().release();
        super.onDestroy();
    }
}
