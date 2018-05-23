package com.vmloft.develop.library.ffmpeg.example;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.VideoView;
import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;
import com.vmloft.develop.library.ffmpeg.example.router.Router;
import com.vmloft.develop.library.tools.router.VMParams;
import com.vmloft.develop.library.tools.utils.VMStrUtil;

/**
 * Created by lzan13 on 2018/5/9.
 * 视频播放界面
 */
public class PlayVideoActivity extends AppActivity {

    @BindView(R.id.btn_play) Button playBtn;
    @BindView(R.id.btn_pause) Button pauseBtn;
    @BindView(R.id.btn_stop) Button stopBtn;

    @BindView(R.id.video_view) VideoView videoView;

    private String videoPath = "/sdcard/Download/wxrecord.mp4";

    @Override protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_play_video);

        ButterKnife.bind(activity);

        init();
    }

    private void init() {
        VMParams params = Router.getParams(activity);
        if (!VMStrUtil.isEmpty(params.str0)) {
            videoPath = params.str0;
        }
        videoView.setVideoPath(videoPath);
    }

    @OnClick({ R.id.btn_play, R.id.btn_pause, R.id.btn_stop, R.id.btn_fullscreen })
    public void onClick(View view) {
        switch (view.getId()) {
        case R.id.btn_play:
            videoView.start();
            break;
        case R.id.btn_pause:
            videoView.pause();
            break;
        case R.id.btn_stop:
            videoView.stopPlayback();
            break;
        case R.id.btn_fullscreen:

            break;
        }
    }
}
