//
// Created by 霍振鹏 on 2019-12-24.
// 主要用于解码操作
//

#ifndef RESTUDYAV_HFFMPEG_H
#define RESTUDYAV_HFFMPEG_H

#include "common/CallBackJava.h"
#include "HAudio.h"

extern "C"
{
#include <libavformat/avformat.h>
};

class HFFmpeg {

public:
    //最好全部初始化为NULL

    //需要解码的音视频文件的地址
    const char* url=NULL;

    //用于回调Java层代码的类
    CallBackJava *callBackJava=NULL;

    //解码线程
    pthread_t pthread_decode=NULL;

    HAudio *hAudio=NULL;

    AVFormatContext *avFormatContext=NULL;

    bool LOG_DEBUG=true;

public:

    HFFmpeg(const char* url,CallBackJava *callBackJava);

    ~HFFmpeg();

    /**
     * 解码前的准备工作
     */
     void prepare();

     /**
      * 开始解码操作
      */
      void start();


      /**
       * 解码线程的回调方法
       */
      void decode();





};


#endif //RESTUDYAV_HFFMPEG_H
