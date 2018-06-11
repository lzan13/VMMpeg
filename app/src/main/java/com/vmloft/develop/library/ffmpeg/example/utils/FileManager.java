package com.vmloft.develop.library.ffmpeg.example.utils;

import com.vmloft.develop.library.tools.utils.VMFile;
import java.io.File;
import java.io.FileOutputStream;

public class FileManager {

    public static final String TEST_YUV_FILE = VMFile.getDownload() + "dst.java.yuv";
    public static final String TEST_H264_FILE = VMFile.getDownload() + "dst.java.h264";
    public static final String TEST_PCM_FILE = VMFile.getDownload() + "src.java.pcm";
    public static final String TEST_WAV_FILE = VMFile.getDownload() + "out.java.wav";
    public static final String TEST_AAC_FILE = VMFile.getDownload() + "dst.java.aac";
    private String fileName;
    private FileOutputStream fileOutputStream;
    private boolean testForWrite = true;

    public FileManager(String fileName) {
        this.fileName = fileName;
        try {
            File file = new File(fileName);
            if (file.exists()) {
                file.delete();
            } else {
                file.createNewFile();
            }
            fileOutputStream = new FileOutputStream(file);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void saveFileData(byte[] data, int offset, int length) {
        try {
            fileOutputStream.write(data, offset, length);
            fileOutputStream.flush();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void saveFileData(byte[] data) {
        try {
            fileOutputStream.write(data);
            fileOutputStream.flush();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void closeFile() {
        try {
            fileOutputStream.close();
        } catch (Exception e) {
        } finally {
            try {
                fileOutputStream.close();
            } catch (Exception e1) {
            }
        }
    }
}
