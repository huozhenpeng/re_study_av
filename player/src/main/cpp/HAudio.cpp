//
// Created by 霍振鹏 on 2019-12-24.
//
#include "HAudio.h"

HAudio::HAudio(HPlayStatus *hPlayStatus,CallBackJava *callBackJava) {
    pthread_mutex_init(&pthreadSeekMutex,NULL);
    this->playStatus=hPlayStatus;
    this->hQueue=new HQueue(hPlayStatus);
    this->callBackJava=callBackJava;
    this->sample_rate=44100;
    buffer= (uint8_t *)(av_malloc(44100 * 2 * 2));

}

HAudio::~HAudio() {

}

void * product(void *data) {
    HAudio *hAudio= (HAudio *)(data);
    int count=0;
    while (hAudio->playStatus!=NULL&&!hAudio->playStatus->exit)
    {
        //如果是在seek的话，就暂停解码操作，我们在seek的时候还需要清空队列，所以就不要再解码了
        if(hAudio->playStatus->seek)
        {
            continue;
        }
        /**
         * 这个代码很关键
         * 测试发现解码过程还是比较快的，如果在seek的时候，解码已经完成，seek我们是做了清空队列的操作的，这样的话，
         * seek之后调用av_read_frame的时候，由于已经到了文件末尾就会返回0，最终导致hPlayStatus->exit=true;
         * 所以我们加上这个队列大小限制，不要让一直解码
         */
        if(hAudio->hQueue->getQueueSize()>40)
        {
            continue;
        }
        AVPacket *avPacket=av_packet_alloc();
        //0 if OK, < 0 on error or end of file
        pthread_mutex_lock(&hAudio->pthreadSeekMutex);
        int ret=av_read_frame(hAudio->avFormatContext,avPacket);
        pthread_mutex_unlock(&hAudio->pthreadSeekMutex);
        if(ret==0)
        {
            if(avPacket->stream_index==hAudio->streamIndex)
            {
                if(hAudio->LOG_DEBUG)
                {
                    count++;
                    LOGI("解码第%d帧",count);
                }
                hAudio->hQueue->putAVPacket(avPacket);
            } else{
                av_packet_free(&avPacket);
                av_free(avPacket);
            }
        } else{
            if(hAudio->LOG_DEBUG)
            {
                LOGI("解码出现错误或者到达文件末尾了");
            }
            av_packet_free(&avPacket);
            av_free(avPacket);
            //有疑问，需要试验
            //清除队列中的无用缓存,这块代码又疑问，这个一旦出错不会陷入死循环吗
            while (hAudio->playStatus!=NULL&&!hAudio->playStatus->exit)
            {
                if(hAudio->hQueue->getQueueSize()>0)
                {
                    continue;
                } else{
                    hAudio->playStatus->exit=true;
                    break;
                }
            }
        }
    }
    if(hAudio->LOG_DEBUG)
    {
        LOGI("解码完成");
    }
    pthread_exit(&hAudio->pthread_product);
}

//FILE *outFile=fopen("/storage/emulated/0/voiceplayer.pcm","w");
void * consumer(void *data) {
    HAudio *hAudio= (HAudio *)(data);
    hAudio->initOpenSLES();
    pthread_exit(&hAudio->pthread_consumer);
}


void HAudio::start() {
    pthread_create(&pthread_product,NULL,product,this);
    pthread_create(&pthread_consumer,NULL,consumer,this);
}

int data_size=0;
int resampleAudio(HAudio *hAudio) {
    if(hAudio->LOG_DEBUG)
    {
        LOGI("开始写入pcm数据");
    }
    //这儿虽然用了个while，但是后面每次都有break，写个if就行
    //开始写while不是因为播放，是写入文件用的
    while (hAudio->playStatus!=NULL&&!hAudio->playStatus->exit)
    {
        if(hAudio->LOG_DEBUG)
        {
            LOGI("消费者进入循环");
        }
        if(hAudio->playStatus->seek)
        {
            continue;
        }
        AVPacket *avPacket=av_packet_alloc();
        if(hAudio->hQueue->getAVPacket(avPacket)!=0)
        {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket=NULL;
            continue;
        }
        int ret=0;
        //0 on success
        ret=avcodec_send_packet(hAudio->avCodecContext,avPacket);
        if(ret!=0)
        {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket=NULL;
            continue;
        }

        AVFrame *avFrame=av_frame_alloc();
        ret=avcodec_receive_frame(hAudio->avCodecContext,avFrame);

        //当前AVFrame中存放的时间(比如说该Frame出现在2分钟的时候，那么它的值就是2分钟)
        double  now_time=avFrame->pts*av_q2d(hAudio->time_base);
        //clock表示的是从开始播放到现在已经播放的时长
        //时间校准
        if(now_time<hAudio->clock)
        {
           now_time=hAudio->clock;
        }
        hAudio->clock=now_time;

        if(ret==0)
        {

            if(avFrame->channels>0&&avFrame->channel_layout==0)
            {
                avFrame->channel_layout=av_get_default_channel_layout(avFrame->channels);
            }
            else if(avFrame->channels==0&&avFrame->channel_layout>0)
            {
                avFrame->channel_layout=av_get_channel_layout_nb_channels(avFrame->channel_layout);
            }

            SwrContext *swrContext=NULL;
            //有疑问，需要试验
            swrContext=swr_alloc_set_opts(
                    NULL,
                    AV_CH_LAYOUT_STEREO,//立体声
                    AV_SAMPLE_FMT_S16,//16位
                    avFrame->sample_rate,
                    avFrame->channel_layout,
                    (AVSampleFormat)(avFrame->format),
                    avFrame->sample_rate,
                    NULL,
                    NULL
            );
            if(!swrContext||swr_init(swrContext)<0)
            {
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket=NULL;

                av_frame_free(&avFrame);
                av_free(avFrame);
                avFrame=NULL;
                continue;
            }

            //有疑问，需要试验
            int nb=swr_convert(
                    swrContext,
                    &hAudio->buffer,
                    avFrame->nb_samples,
                    (const uint8_t **)(avFrame->data),
                    avFrame->nb_samples
            );

            int out_channels=av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);

            data_size=nb*out_channels*av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

            //fwrite(hAudio->buffer,1,data_size,outFile);

            if(hAudio->LOG_DEBUG)
            {
                LOGI("data_size is %d",data_size);
            }

            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket=NULL;

            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame=NULL;

            swr_free(&swrContext);
            break;//记得加上这个

        } else{
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket=NULL;

            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame=NULL;
            continue;
        }
    }
    return data_size;
}


SLuint32 getCurrentSampleRateForOpensles(int sample_rate) {
    SLuint32 rate = 0;
    switch (sample_rate)
    {
        case 8000:
            rate = SL_SAMPLINGRATE_8;
            break;
        case 11025:
            rate = SL_SAMPLINGRATE_11_025;
            break;
        case 12000:
            rate = SL_SAMPLINGRATE_12;
            break;
        case 16000:
            rate = SL_SAMPLINGRATE_16;
            break;
        case 22050:
            rate = SL_SAMPLINGRATE_22_05;
            break;
        case 24000:
            rate = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            rate = SL_SAMPLINGRATE_32;
            break;
        case 44100:
            rate = SL_SAMPLINGRATE_44_1;
            break;
        case 48000:
            rate = SL_SAMPLINGRATE_48;
            break;
        case 64000:
            rate = SL_SAMPLINGRATE_64;
            break;
        case 88200:
            rate = SL_SAMPLINGRATE_88_2;
            break;
        case 96000:
            rate = SL_SAMPLINGRATE_96;
            break;
        case 192000:
            rate = SL_SAMPLINGRATE_192;
            break;
        default:
            rate =  SL_SAMPLINGRATE_44_1;
    }
    return rate;
}

void pcmBufferCallBack(SLAndroidSimpleBufferQueueItf bf, void * context)
{
    HAudio *hAudio = (HAudio *) context;
    if(hAudio != NULL)
    {
        int bufferSize = resampleAudio(hAudio);
        if(bufferSize > 0)
        {
            //不强制转换，会有异常，其实java层定义为int类型即可
            hAudio->clock+=bufferSize/((double)(hAudio->sample_rate*2*2));
            //我们0.1s回调一次
            if(hAudio->clock-hAudio->last_time>0.1)
            {
                hAudio->last_time=hAudio->clock;
                if(hAudio->LOG_DEBUG)
                {
                    LOGI("时间回调");
                }
                hAudio->callBackJava->onShowTime(CHILD_THREAD,200,hAudio->duration,hAudio->clock);

            }

            (* hAudio-> pcmBufferQueue)->Enqueue( hAudio->pcmBufferQueue, (char *) hAudio-> buffer, bufferSize);
        }
    }
}

void HAudio::initOpenSLES() {

    //第一步：创建引擎
    SLresult result;
    result=slCreateEngine(&engineObject,0,0,0,0,0);
    result=(*engineObject)->Realize(engineObject,SL_BOOLEAN_FALSE);
    result=(*engineObject)->GetInterface(engineObject,SL_IID_ENGINE,&engineEngine);

    //第二步：创建混音器
    const  SLInterfaceID  mids[1]={SL_IID_ENVIRONMENTALREVERB};
    const SLboolean  mreq[1]={SL_BOOLEAN_FALSE};
    result=(*engineEngine)->CreateOutputMix(engineEngine,&outputMixObject,1,mids,mreq);

    (void)result;
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    (void)result;
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB, &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS == result) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
        (void)result;
    }
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&outputMix, 0};


    // 第三步，配置PCM格式信息
    SLDataLocator_AndroidSimpleBufferQueue android_queue={SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,2};

    SLDataFormat_PCM pcm={
            SL_DATAFORMAT_PCM,//播放pcm格式的数据
            2,//2个声道（立体声）
            getCurrentSampleRateForOpensles(sample_rate),//44100hz的频率
            SL_PCMSAMPLEFORMAT_FIXED_16,//位数 16位
            SL_PCMSAMPLEFORMAT_FIXED_16,//和位数一致就行
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//立体声（前左前右）
            SL_BYTEORDER_LITTLEENDIAN//结束标志
    };
    SLDataSource slDataSource = {&android_queue, &pcm};


    const SLInterfaceID ids[1] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};

    (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayerObject, &slDataSource, &audioSnk, 1, ids, req);
    //初始化播放器
    (*pcmPlayerObject)->Realize(pcmPlayerObject, SL_BOOLEAN_FALSE);

//    得到接口后调用  获取Player接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_PLAY, &pcmPlayerPlay);

//    注册回调缓冲区 获取缓冲队列接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_BUFFERQUEUE, &pcmBufferQueue);
    //缓冲接口回调
    (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallBack, this);
//    获取播放状态接口
    (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);

    pcmBufferCallBack(pcmBufferQueue, this);


}


void HAudio::pause() {

    if(pcmPlayerPlay!=NULL)
    {
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay,SL_PLAYSTATE_PAUSED);
    } else{
        if(LOG_DEBUG)
        {
            LOGI("pcmPlayerPlay 为空");
        }
    }

}

void HAudio::resume() {
    if(pcmPlayerPlay!=NULL)
    {
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay,SL_PLAYSTATE_PLAYING);
    } else{
        if(LOG_DEBUG)
        {
            LOGI("pcmPlayerPlay 为空");
        }
    }
}

void HAudio::stop() {
    if(pcmPlayerPlay!=NULL)
    {
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay,SL_PLAYSTATE_STOPPED);
    } else{
        if(LOG_DEBUG)
        {
            LOGI("pcmPlayerPlay 为空");
        }
    }

}

void HAudio::seek(int second) {

    if(duration<=0)
    {
        return;
    }
    if(second>=0&&second<=duration)
    {
        playStatus->seek=true;
        //清空队列，要不然队列中有残余数据
        hQueue->clearAvPacket();
        clock=0;
        last_time=0;
        //在执行清空操作的时候，让生成packet的线程先阻塞
        pthread_mutex_lock(&pthreadSeekMutex);
        int64_t rel=second*AV_TIME_BASE;
        avformat_seek_file(avFormatContext,-1,INT64_MIN,rel,INT64_MAX,0);
        pthread_mutex_unlock(&pthreadSeekMutex);
        playStatus->seek= false;
    }

}

void HAudio::release() {
    if(hQueue!=NULL)
    {
        delete(hQueue);
        hQueue=NULL;
    }
    if(pcmPlayerObject!=NULL)
    {
        (*pcmPlayerObject)->Destroy(pcmPlayerObject);
        pcmPlayerObject=NULL;
        pcmPlayerPlay=NULL;
        pcmBufferQueue=NULL;
    }
    if(outputMixObject!=NULL)
    {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject=NULL;
        outputMixEnvironmentalReverb=NULL;
    }
    if(engineObject!=NULL)
    {
        (*engineObject)->Destroy(engineObject);
        engineObject=NULL;
        engineEngine=NULL;
    }
    if(buffer!=NULL)
    {
        free(buffer);
        buffer=NULL;
    }
    if(avCodecContext!=NULL)
    {
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext=NULL;
    }
    if(playStatus!=NULL)
    {
        playStatus=NULL;
    }
    if(callBackJava!=NULL)
    {
        callBackJava=NULL;
    }


}







