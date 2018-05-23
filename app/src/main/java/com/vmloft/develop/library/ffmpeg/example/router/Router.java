package com.vmloft.develop.library.ffmpeg.example.router;

import android.content.Context;

import com.vmloft.develop.library.ffmpeg.example.CameraActivity;
import com.vmloft.develop.library.ffmpeg.example.ConfigActivity;
import com.vmloft.develop.library.ffmpeg.example.PlayVideoActivity;
import com.vmloft.develop.library.tools.router.VMParams;
import com.vmloft.develop.library.tools.router.VMRouter;

/**
 * Created by lzan13 on 2018/5/10.
 * 项目路由，界面跳转都由此类管理
 */
public class Router extends VMRouter {

    /**
     * 跳转到摄像界面
     */
    public static void goConfig(Context context) {
        overlay(context, ConfigActivity.class);
    }

    /**
     * 跳转到摄像界面
     */
    public static void goCamera(Context context) {
        overlay(context, CameraActivity.class);
    }

    /**
     * 跳转到播放界面
     */
    public static void goPlay(Context context, VMParams params) {
        overlay(context, PlayVideoActivity.class, params);
    }
}

