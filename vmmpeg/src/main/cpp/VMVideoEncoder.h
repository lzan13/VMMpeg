//
// Created by lz liu on 2018/6/7.
//

#ifndef VMMPEG_VMVIDEOENCODER_H
#define VMMPEG_VMVIDEOENCODER_H


extern "C" {
#include <libavutil/pixfmt.h>
#include <libavcodec/avcodec.h>
#include <x264.h>
}

class VMVideoEncoder {

private:
    int width;
    int height;
    /* e.g. 25, 60, etc.. */
    int fps;
    int bitrate;
    int i_threads;
    int i_vbv_buffer_size;
    int i_slice_max_size;
    int b_frame_frq;

    /* x264 struct*/
    x264_picture_t pic_in;
    x264_picture_t pic_out;
    x264_param_t params;
    x264_nal_t *nals;
    x264_t *encoder;
    int num_nals;

    FILE *out1;
    FILE *out2;

public:
    VMVideoEncoder();
    ~VMVideoEncoder();
    /* open for encoding */
    bool open();
    /* encode the given data */
    int encodeFrame(uint8_t *inBytes, int frameSize, int pts, uint8_t *outBytes, int *outFrameSize);
    /* close the encoder and file, frees all memory */
    bool close();
    /* sets the x264 params */
    void initX264Params();
    int getFps() const;
    void setFps(int fps);
    int getNumNals() const;
    void setNumNals(int numNals);
    int getHeight() const;
    void setHeight(int outHeight);
    int getWidth() const;
    void setWidth(int outWidth);
    int getBitrate() const;
    void setBitrate(int bitrate);
    int getSliceMaxSize() const;
    void setSliceMaxSize(int sliceMaxSize);
    int getVbvBufferSize() const;
    void setVbvBufferSize(int vbvBufferSize);
    int getIThreads() const;
    void setIThreads(int threads);
    int getBFrameFrq() const;
    void setBFrameFrq(int frameFrq);
};


#endif //VMMPEG_VMVIDEOENCODER_H
