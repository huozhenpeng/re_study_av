//
// Created by 霍振鹏 on 2019-12-24.
//
#include "HAudio.h"
#include "head/log.h"
#include "HOpenslEs.h"

extern "C"
{
#include <libswresample/swresample.h>
}

HAudio::HAudio(HPlayStatus *hPlayStatus) {
    this->playStatus=hPlayStatus;
    this->hQueue=new HQueue(hPlayStatus);
    buffer= (uint8_t *)(av_malloc(44100 * 2 * 2));
}

HAudio::~HAudio() {

}

void * product(void *data) {
    HAudio *hAudio= (HAudio *)(data);
    int count=0;
    while (hAudio->playStatus!=NULL&&!hAudio->playStatus->exit)
    {
        AVPacket *avPacket=av_packet_alloc();
        //0 if ok
        if(av_read_frame(hAudio->avFormatContext,avPacket)==0)
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
                LOGI("解码出现错误");
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
    HOpenslEs *hOpenslEs=new HOpenslEs(hAudio,44100);
    hOpenslEs->initOpenSLES();
    pthread_exit(&hAudio->pthread_consumer);
}


void HAudio::start() {
    pthread_create(&pthread_product,NULL,product,this);
    pthread_create(&pthread_consumer,NULL,consumer,this);
}

int HAudio::resampleAudio(HAudio *hAudio) {
    if(hAudio->LOG_DEBUG)
    {
        LOGI("开始写入pcm数据");
    }
    while (hAudio->playStatus!=NULL&&!hAudio->playStatus->exit)
    {
        if(hAudio->LOG_DEBUG)
        {
            LOGI("消费者进入循环");
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

        } else{
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket=NULL;

            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame=NULL;
            continue;
        }
        return data_size;
    }
}





