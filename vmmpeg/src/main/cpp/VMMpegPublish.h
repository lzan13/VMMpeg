//
// Created by lz liu on 2018/6/11.
//

#ifndef VMMPEG_VMMPEGPUBLISH_H
#define VMMPEG_VMMPEGPUBLISH_H

extern "C" {
#include "librtmp/rtmp.h"
};

class VMMpegPublish {

public:
    unsigned char *rtmp_url;
    unsigned int start_time;
    RTMP *rtmp;

    VMMpegPublish();
    ~VMMpegPublish();

    void init(unsigned char *url);

    void addSequenceAacHeader(int timestamp, int sampleRate, int channel);
    void addAccBody(unsigned char *buf, int len, long timeStamp);
    void addSequenceH264Header(unsigned char *sps, int sps_len, unsigned char *pps, int pps_len);
    void addH264Body(unsigned char *buf, int len, long timeStamp);
    int getSampleRateIndex(int sampleRate);

    void release();
};


#endif //VMMPEG_VMMPEGPUBLISH_H
