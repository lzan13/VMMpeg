package com.vmloft.develop.library.ffmpeg.example;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.support.v7.widget.Toolbar;
import android.view.ContextThemeWrapper;
import android.view.View;
import android.widget.Button;
import android.widget.RelativeLayout;
import android.widget.TextView;

import butterknife.BindView;
import butterknife.ButterKnife;

import com.vmloft.develop.library.ffmpeg.VMEncoder;
import com.vmloft.develop.library.ffmpeg.VMFFmpeg;
import com.vmloft.develop.library.ffmpeg.VMMuxer;
import com.vmloft.develop.library.ffmpeg.example.router.Router;
import com.vmloft.develop.library.tools.router.VMParams;
import com.vmloft.develop.library.tools.utils.VMFileUtil;
import com.vmloft.develop.library.tools.utils.VMStrUtil;
import com.vmloft.develop.library.tools.widget.VMToast;
import com.vmloft.develop.library.tools.widget.VMViewGroup;

public class MainActivity extends AppActivity {

    @BindView(R.id.widget_toolbar) Toolbar toolbar;
    @BindView(R.id.text_file_path) TextView filepathView;
    @BindView(R.id.view_group) VMViewGroup viewGroup;
    @BindView(R.id.layout_progress) RelativeLayout progressLayout;

    private final int CODE_SELECT_FILE = 100;
    private String inPath;
    private String outPath;
    private String encode = "h264";
    private String width = "640";
    private String height = "480";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ButterKnife.bind(activity);

        init();
    }

    private void init() {
        setSupportActionBar(toolbar);
        toolbar.setTitle("MainActivity");

        String[] btnArray = { "选择文件", "FFmpeg 信息", "录制", "播放" };
        for (int i = 0; i < btnArray.length; i++) {
            Button btn = new Button(new ContextThemeWrapper(activity, R.style.VMBtn_Green_Fillet), null, 0);
            btn.setText(btnArray[i]);
            btn.setId(100 + i);
            btn.setOnClickListener(viewListener);
            viewGroup.addView(btn);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent intent) {
        //super.onActivityResult(requestCode, resultCode, data);
        if (resultCode != RESULT_OK) {
            return;
        }
        if (requestCode == CODE_SELECT_FILE) {
            Uri uri = intent.getData();
            inPath = VMFileUtil.getPath(uri);
            filepathView.setText(inPath);
        }
    }

    private View.OnClickListener viewListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            switch (v.getId()) {
            case 100:
                openSelectFile();
                break;
            case 101:
                Router.goConfig(activity);
                break;
            case 102:
                Router.goCamera(activity);
                break;
            case 103:
                VMParams params = new VMParams();
                params.str0 = inPath;
                Router.goPlay(activity, params);
                break;
            case 104:
                break;
            }
        }
    };

    /**
     * 打开文件选择器
     */
    private void openSelectFile() {
        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
        intent.setType("*/*");
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        startActivityForResult(intent, CODE_SELECT_FILE);
    }

    /**
     * 检查读取文件是否存在
     */
    private boolean checkFilepath() {
        if (VMStrUtil.isEmpty(inPath)) {
            VMToast.make("没有选择文件，请先选择文件").showError();
            return false;
        }
        return true;
    }

    /**
     * 解析写入路径
     */
    private void parseWritePath() {
        String path = inPath.substring(0, inPath.lastIndexOf("/") + 1);
        String filename = inPath.substring(inPath.lastIndexOf("/") + 1, inPath.lastIndexOf("."));
        String[] nameArr = VMStrUtil.strToArray(filename, "\\.");
        width = nameArr[1];
        height = nameArr[2];
        outPath = path + filename + "." + encode;
    }

    /**
     * 加载底层库
     */
    static {
        System.loadLibrary("vmmpeg");
    }
}
