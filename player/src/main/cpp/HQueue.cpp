//
// Created by 霍振鹏 on 2019-12-24.
//

#include "HQueue.h"
#include "head/log.h"

HQueue::HQueue(HPlayStatus *playStatus) {

    this->playStatus=playStatus;
    pthread_mutex_init(&pthreadMutex,NULL);
    pthread_cond_init(&pthreadCond,NULL);
}

HQueue::~HQueue() {

}

void HQueue::putAVPacket(AVPacket *avPacket) {

    //上锁
    pthread_mutex_lock(&pthreadMutex);
    queue.push(avPacket);
    if(LOG_DEBUG)
    {
        LOGI("队列中的个数:%d",queue.size());
    }

    //通知消费者，但不会释放锁
    pthread_cond_signal(&pthreadCond);
    //释放锁
    pthread_mutex_unlock(&pthreadMutex);
}

int HQueue::getAVPacket(AVPacket *avPacket) {
    pthread_mutex_lock(&pthreadMutex);
    while (playStatus!=NULL&&!playStatus->exit)
    {
        if(queue.size()>0)
        {
            AVPacket *packet=queue.front();

            //return 0 on success
            //Setup a new reference to the data described by a given packet
            //感觉相当于是入参出参的一个作用
            if(av_packet_ref(avPacket,packet)==0)
            {
                queue.pop();
            }

            av_packet_free(&packet);
            av_free(packet);
            packet=NULL;
            if(LOG_DEBUG)
            {
                LOGI("从队列中取出一个AVPacket，还剩余%d个",queue.size());
            }
            break;
        } else{
            //等待，并释放锁
            pthread_cond_wait(&pthreadCond,&pthreadMutex);
        }
    }
    pthread_mutex_unlock(&pthreadMutex);
    return 0;
}

/**
 * 完了试验下能用不
 * @return
 */
AVPacket* HQueue::getAVPacket2() {
    pthread_mutex_lock(&pthreadMutex);
    while (playStatus!=NULL&&!playStatus->exit)
    {
        if(queue.size()>0)
        {
            AVPacket *packet=queue.front();

            if(packet!=NULL)
            {
                queue.pop();
            }

            if(LOG_DEBUG)
            {
                LOGI("从队列中取出一个AVPacket，还剩余%d个",queue.size());
            }
            return packet;
        } else{
            //等待，并释放锁
            pthread_cond_wait(&pthreadCond,&pthreadMutex);
        }
    }
    return NULL;
}

int HQueue::getQueueSize() {
    int size=0;
    pthread_mutex_lock(&pthreadMutex);
    size=queue.size();
    pthread_mutex_unlock(&pthreadMutex);
    return size;
}
