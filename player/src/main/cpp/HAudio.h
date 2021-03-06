//
// Created by 霍振鹏 on 2019-12-24.
// 负责记录解码后获取到的音频通道索引号、解码器上下文、解码器参数，音频重采样
//

#ifndef RESTUDYAV_HAUDIO_H
#define RESTUDYAV_HAUDIO_H

#include "HQueue.h"
#include "head/log.h"
#include "common/CallBackJava.h"
#include "SoundTouch.h"
using namespace soundtouch;

extern "C"
{
#include "libavcodec/avcodec.h"
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <libavutil/time.h>
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
    //音量控制
    SLVolumeItf  pcmVolumePlay;

    //控制声道
    SLMuteSoloItf  pcmPlayPlayerMuteSolo;

    SLAndroidSimpleBufferQueueItf pcmBufferQueue;


    int duration=0;
    AVRational time_base;
    double clock=0;
    double  last_time=0;

    CallBackJava *callBackJava;

    //当前音量
    int volumePercent;

    //SoundTouch
    SoundTouch *soundTouch = NULL;
    SAMPLETYPE *sampleBuffer = NULL;
    bool finished = true;
    uint8_t *out_buffer = NULL;
    int nb = 0;
    int num = 0;

    float pitch = 1.0f;
    float speed = 1.0f;

    //记录是否录制
    jboolean  isRecordPcm= false;

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

    //音量控制
    void setVolume(int percent);

    //立体声
    void setStereoVolume();

    //右声道
    void setRightVolume();

    //左声道
    void setLeftVolume();

    //设置音速
    void setSpeed(jfloat speed);

    //设置音调
    void setTonal(jfloat tonal);

    int getSoundTouchData();

    //获取分贝值
    int getPCMDB(char *pcmcata, size_t pcmsize);

    void startRecord(jboolean record);


};


#endif //RESTUDYAV_HAUDIO_H
