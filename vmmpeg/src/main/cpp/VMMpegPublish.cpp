//
// Created by lz liu on 2018/6/11.
//

#include <string.h>
#include <malloc.h>
#include <x264.h>

#include "VMLog.h"
#include "VMMpegPublish.h"


#define RTMP_HEAD_SIZE   (sizeof(RTMPPacket) + RTMP_MAX_HEADER_SIZE)


VMMpegPublish::VMMpegPublish() {}

VMMpegPublish::~VMMpegPublish() {}

/**
 * 初始化RTMP数据，与rtmp连接
 * @param url
 */
void VMMpegPublish::init(unsigned char *url) {
    this->rtmp_url = url;
    rtmp = RTMP_Alloc();
    RTMP_Init(rtmp);

    rtmp->Link.timeout = 5;
    RTMP_SetupURL(rtmp, (char *) url);
    RTMP_EnableWrite(rtmp);

    //建立RTMP socket连接
    if (!RTMP_Connect(rtmp, NULL)) {
        LOGE("RTMP Socket 连接失败");
    } else {
        LOGI("RTMP Socket 连接成功");
    }
    //连接到rtmp流上
    if (!RTMP_ConnectStream(rtmp, 0)) {
        LOGE("RTMP 流连接失败");
    } else {
        LOGI("RTMP 流连接成功");
    }
    start_time = RTMP_GetTime();
    LOGI("开始时间 = %d", start_time);
}

/**
 * 发送sps pps，即发送H264SequenceHead头
 * @param sps
 * @param sps_len
 * @param pps
 * @param pps_len
 */
void VMMpegPublish::addSequenceH264Header(unsigned char *sps, int sps_len, unsigned char *pps, int pps_len) {
    //LOGI("#######addSequenceH264Header#########pps_lem=%d, sps_len=%d", pps_len, sps_len);
    LOGI("发送视频数据头开始");
    RTMPPacket *packet = (RTMPPacket *) malloc(RTMP_HEAD_SIZE + 1024);
    memset(packet, 0, RTMP_HEAD_SIZE + 1024);
    packet->m_body = (char *) packet + RTMP_HEAD_SIZE;

    unsigned char *body = (unsigned char *) packet->m_body;

    int i = 0;
    /*1:keyframe 7:AVC*/
    body[i++] = 0x17;
    /* AVC head */
    body[i++] = 0x00;

    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00;

    /*AVCDecoderConfigurationRecord*/
    //configurationVersion版本号，1
    body[i++] = 0x01;
    //AVCProfileIndication sps[1]
    body[i++] = sps[1];
    //profile_compatibility sps[2]
    body[i++] = sps[2];
    //AVCLevelIndication sps[3]
    body[i++] = sps[3];
    //6bit的reserved为二进制位111111和2bitlengthSizeMinusOne一般为3，
    //二进制位11，合并起来为11111111，即为0xff
    body[i++] = 0xff;

    /*sps*/
    //3bit的reserved，二进制位111，5bit的numOfSequenceParameterSets，
    //sps个数，一般为1，及合起来二进制位11100001，即为0xe1
    body[i++] = 0xe1;
    //SequenceParametersSetNALUnits（sps_size + sps）的数组
    body[i++] = (sps_len >> 8) & 0xff;
    body[i++] = sps_len & 0xff;
    memcpy(&body[i], sps, sps_len);
    i += sps_len;

    /*pps*/
    //numOfPictureParameterSets一般为1，即为0x01
    body[i++] = 0x01;
    //SequenceParametersSetNALUnits（pps_size + pps）的数组
    body[i++] = (pps_len >> 8) & 0xff;
    body[i++] = (pps_len) & 0xff;
    memcpy(&body[i], pps, pps_len);
    i += pps_len;

    //Message Type，RTMP_PACKET_TYPE_VIDEO：0x09
    packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
    //Payload Length
    packet->m_nBodySize = i;
    //Time Stamp：4字节
    //记录了每一个tag相对于第一个tag（File Header）的相对时间。
    //以毫秒为单位。而File Header的time stamp永远为0。
    packet->m_nTimeStamp = 0;
    packet->m_hasAbsTimestamp = 0;
    //Channel ID，Audio和Vidio通道
    packet->m_nChannel = 0x04;
    packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
    packet->m_nInfoField2 = rtmp->m_stream_id;

    //send rtmp
    if (RTMP_IsConnected(rtmp)) {
        RTMP_SendPacket(rtmp, packet, TRUE);
        LOGI("send packet sendSpsAndPps");
    }
    LOGI("发送视频数据头完成");
    free(packet);
}

/**
 * 发送H264数据
 * @param buf
 * @param len
 * @param timeStamp
 */
void VMMpegPublish::addH264Body(unsigned char *buf, int len, long timeStamp) {
    LOGI("发送视频数据开始");
    //去掉起始码(界定符)
    if (buf[2] == 0x00) {
        //00 00 00 01
        buf += 4;
        len -= 4;
    } else if (buf[2] == 0x01) {
        // 00 00 01
        buf += 3;
        len -= 3;
    }
    int body_size = len + 9;
    RTMPPacket *packet = (RTMPPacket *) malloc(RTMP_HEAD_SIZE + 9 + len);
    memset(packet, 0, RTMP_HEAD_SIZE);
    packet->m_body = (char *) packet + RTMP_HEAD_SIZE;

    unsigned char *body = (unsigned char *) packet->m_body;
    //当NAL头信息中，type（5位）等于5，说明这是关键帧NAL单元
    //buf[0] NAL Header与运算，获取type，根据type判断关键帧和普通帧
    //00000101 & 00011111(0x1f) = 00000101
    int type = buf[0] & 0x1f;
    //Pframe  7:AVC
    body[0] = 0x27;
    //IDR I帧图像
    //Iframe  7:AVC
    if (type == NAL_SLICE_IDR) {
        body[0] = 0x17;
    }
    //AVCPacketType = 1
    /*nal unit,NALUs（AVCPacketType == 1)*/
    body[1] = 0x01;
    //composition time 0x000000 24bit
    body[2] = 0x00;
    body[3] = 0x00;
    body[4] = 0x00;

    //写入NALU信息，右移8位，一个字节的读取
    body[5] = (len >> 24) & 0xff;
    body[6] = (len >> 16) & 0xff;
    body[7] = (len >> 8) & 0xff;
    body[8] = (len) & 0xff;

    /*copy data*/
    memcpy(&body[9], buf, len);

    packet->m_hasAbsTimestamp = 0;
    packet->m_nBodySize = body_size;
    //当前packet的类型：Video
    packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
    packet->m_nChannel = 0x04;
    packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
    packet->m_nInfoField2 = rtmp->m_stream_id;
    //记录了每一个tag相对于第一个tag（File Header）的相对时间
    packet->m_nTimeStamp = RTMP_GetTime() - start_time;

    //send rtmp h264 body data
    if (RTMP_IsConnected(rtmp)) {
        RTMP_SendPacket(rtmp, packet, TRUE);
        //LOGD("send packet sendVideoData");
    }
    LOGI("发送视频数据完成");
    free(packet);
}

int VMMpegPublish::getSampleRateIndex(int sampleRate) {
    int sampleRateArray[] = {96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000,
        11025, 8000, 7350};
    for (int i = 0; i < 13; ++i) {
        if (sampleRateArray[i] == sampleRate) {
            return i;
        }
    }
    return -1;
}

void VMMpegPublish::release() {
    RTMP_Close(rtmp);
    RTMP_Free(rtmp);
}