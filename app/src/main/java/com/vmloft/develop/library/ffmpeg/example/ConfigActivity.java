package com.vmloft.develop.library.ffmpeg.example;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import com.vmloft.develop.library.ffmpeg.VMFFmpeg;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

public class ConfigActivity extends AppActivity {

    @BindView(R.id.text_view) TextView textView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_config);

        ButterKnife.bind(activity);

    }

    @OnClick({R.id.btn_protocol, R.id.btn_codec, R.id.btn_filter, R.id.btn_format})
    public void onClick(View view) {
        String config = "";
        switch (view.getId()) {
        case R.id.btn_protocol:
            config = VMFFmpeg.protocolInfo();
            break;
        case R.id.btn_codec:
            config = VMFFmpeg.codecInfo();
            break;
        case R.id.btn_filter:
            config = VMFFmpeg.filterInfo();
            break;
        case R.id.btn_format:
            config = VMFFmpeg.formatInfo();
            break;
        }
        textView.setText(config);

    }
}
