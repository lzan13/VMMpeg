package com.vmloft.develop.library.ffmpeg.example;

import com.vmloft.develop.library.tools.VMApp;
import com.vmloft.develop.library.tools.VMTools;

/**
 * Created by lzan13 on 2018/5/10.
 */
public class App extends VMApp {

    @Override public void onCreate() {
        super.onCreate();

        VMTools.init(context);
    }
}
