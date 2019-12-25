//
// Created by 霍振鹏 on 2019-12-24.
//

#include "HFFmpeg.h"
#include "head/log.h"

HFFmpeg::HFFmpeg(const char *url, CallBackJava *callBackJava,HPlayStatus *hPlayStatus) {

    this->url=url;
    this->callBackJava=callBackJava;
    this->playStatus=hPlayStatus;
}

HFFmpeg::~HFFmpeg() {

}

//如果不这样传参数的话，decode方法，只有在头文件中声明的方法才可以直接访问
void *decodeFFmpeg(void *data)
{
    HFFmpeg *hfFmpeg= (HFFmpeg *)(data);
    hfFmpeg->decode();
    //需要释放不
    pthread_exit(&hfFmpeg->pthread_decode);
}


void HFFmpeg::prepare() {

    //初始化解码准备工作线程
    pthread_create(&pthread_decode,NULL,decodeFFmpeg,this);
}



void HFFmpeg::start() {

    if(hAudio==NULL)
    {
        LOGI("audio对象不能为NULL");
        return;
    }
    hAudio->start();

}

void HFFmpeg::decode() {

    av_register_all();
    //获取网络流的时候需要用到
    avformat_network_init();

    //第一步：打开流
    avFormatContext=avformat_alloc_context();
    //0 if success
    int result=0;
    result=avformat_open_input(&avFormatContext,url,NULL,NULL);
    if(result!=0)
    {
        if(LOG_DEBUG)
        {
            LOGI("打开媒体文件失败");
        }
        //需要释放不
        return;
    }


    //第二步：查找流信息
    result=avformat_find_stream_info(avFormatContext,NULL);
    if(result<0)
    {
        if(LOG_DEBUG)
        {
            LOGI("无法找到流信息：%s",url);
        }
        //需要释放不
        return;;
    }

    //第三步：查找音频索引，忽略视频索引、字幕索引
    for(int i=0;i<avFormatContext->nb_streams;i++)
    {
        if(avFormatContext->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_AUDIO)
        {
            //将相关音频信息记录到HAudio类中
            if(hAudio==NULL)
            {
                hAudio=new HAudio(playStatus,callBackJava);
                hAudio->streamIndex=i;
                hAudio->avCodecParameters=avFormatContext->streams[i]->codecpar;
                hAudio->avFormatContext=avFormatContext;


                //统计播放时长
                hAudio->duration=avFormatContext->duration/AV_TIME_BASE;
                hAudio->time_base=avFormatContext->streams[i]->time_base;
            }
        }
    }

    //第四步：获取音频的解码器
    AVCodec *avCodec=avcodec_find_decoder(hAudio->avCodecParameters->codec_id);
    if(avCodec==NULL)
    {
        if(LOG_DEBUG)
        {
            LOGI("获取音频解码器失败");
        }
        //需要释放不
        return;
    }

    //第五步：创建解码器上下文
    hAudio->avCodecContext=avcodec_alloc_context3(avCodec);
    if(hAudio->avCodecContext==NULL)
    {
        if(LOG_DEBUG)
        {
            LOGI("创建解码器上下文失败");
        }
        //需要释放不
        return;
    }

    //第六步：将音频流信息拷贝到上面创建的解码器上下文中
    if(avcodec_parameters_to_context(hAudio->avCodecContext,hAudio->avCodecParameters))
    {
        if(LOG_DEBUG)
        {
            LOGI("拷贝失败");
        }
        //需要释放不
        return;
    }

    //第七步：打开解码器 0 if success
    if(avcodec_open2(hAudio->avCodecContext,avCodec,NULL)!=0)
    {
        if(LOG_DEBUG)
        {
            LOGI("代码解码器失败");
        }
        //需要释放不
        return;
    }

    //解码音频的准备工作已经完成，可以通知java层开始播放了
    const char* msg="解码音频的准备工作已经完成";
    callBackJava->onCallBack(0,200,msg);
}

void HFFmpeg::pause() {
    hAudio->pause();

}

void HFFmpeg::resume() {
    hAudio->resume();

}

void HFFmpeg::stop() {

    hAudio->stop();
}

void HFFmpeg::release() {

    if(LOG_DEBUG)
    {
        LOGI("开始释放FFmpeg");
    }
    if(playStatus->exit)
    {
        return;
    }
    playStatus->exit=true;

    if(hAudio!=NULL)
    {
        LOGI("开始释放audio");
        hAudio->release();
        delete(hAudio);
        hAudio=NULL;
    }
    if(avFormatContext!=NULL)
    {
        LOGI("开始释放avFormatContext");
        avformat_close_input(&avFormatContext);
        avformat_free_context(avFormatContext);
        avFormatContext=NULL;
    }
    if(callBackJava!=NULL)
    {
        LOGI("开始释放callBackJava");
        callBackJava=NULL;
    }
    if(playStatus!=NULL)
    {
        LOGI("开始释放playStatus");
    }
}

void HFFmpeg::seek(int second) {

    hAudio->seek(second);

}





