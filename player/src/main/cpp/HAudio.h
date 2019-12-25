//
// Created by 霍振鹏 on 2019-12-24.
// 负责记录解码后获取到的音频通道索引号、解码器上下文、解码器参数，音频重采样
//

#ifndef RESTUDYAV_HAUDIO_H
#define RESTUDYAV_HAUDIO_H

#include "HQueue.h"
#include "head/log.h"
#include "common/CallBackJava.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
};

class HAudio {

public:
    int streamIndex=-1;
    AVCodecParameters *avCodecParameters=NULL;
    AVCodecContext *avCodecContext=NULL;
    AVFormatContext *avFormatContext=NULL;
    HQueue *hQueue=NULL;
    HPlayStatus *playStatus=NULL;
    //我觉得生产者、消费者都由这个类来负责吧
    pthread_t  pthread_product;
    pthread_t  pthread_consumer;
    uint8_t  *buffer=NULL;

    bool LOG_DEBUG=true;

    pthread_mutex_t pthreadSeekMutex;



    int sample_rate;
    SLObjectItf  engineObject;
    SLEngineItf  engineEngine;
    SLObjectItf outputMixObject;

    SLEnvironmentalReverbItf  outputMixEnvironmentalReverb;
    SLEnvironmentalReverbSettings reverbSettings;

    SLObjectItf pcmPlayerObject;

    SLPlayItf  pcmPlayerPlay;

    SLAndroidSimpleBufferQueueItf pcmBufferQueue;


    int duration=0;
    AVRational time_base;
    double clock=0;
    double  last_time=0;

    CallBackJava *callBackJava;

public:
    HAudio(HPlayStatus *hPlayStatus,CallBackJava *callBackJava);
    ~HAudio();

    void initOpenSLES();

    //线程方法不能是成员方法，要不然报错
//    void * product(void *data);
//    void * consumer(void *data);

    void start();

    void pause();

    void resume();

    void stop();

    void release();

    void seek(int second);

};


#endif //RESTUDYAV_HAUDIO_H
