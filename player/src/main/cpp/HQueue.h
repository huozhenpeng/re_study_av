//
// Created by 霍振鹏 on 2019-12-24.
// 队列：存放解码后的原始压缩数据,提供入队出队操作，生产者消费者同步
//

#ifndef RESTUDYAV_HQUEUE_H
#define RESTUDYAV_HQUEUE_H

#include <queue>
#include <string>
#include "HPlayStatus.h"

using namespace std;

extern "C"
{
#include <libavcodec/avcodec.h>
}

class HQueue {

public:
    pthread_mutex_t pthreadMutex;
    pthread_cond_t pthreadCond;


    //创建一个队列，这就算是创建好了
    std::queue<AVPacket*> queue;
    HPlayStatus *playStatus;

    bool LOG_DEBUG=true;

public:
    HQueue(HPlayStatus *playStatus);
    ~HQueue();

    void putAVPacket(AVPacket *avPacket);
    int getAVPacket(AVPacket *avPacket);
    int getQueueSize();

    AVPacket * getAVPacket2();

    void clearAvPacket();

};


#endif //RESTUDYAV_HQUEUE_H
