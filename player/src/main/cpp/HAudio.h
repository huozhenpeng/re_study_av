//
// Created by 霍振鹏 on 2019-12-24.
// 负责记录解码后获取到的音频通道索引号、解码器上下文、解码器参数
//

#ifndef RESTUDYAV_HAUDIO_H
#define RESTUDYAV_HAUDIO_H

extern "C"
{
#include "libavcodec/avcodec.h"
};

class HAudio {

public:
    int streamIndex=-1;
    AVCodecParameters *avCodecParameters=NULL;
    AVCodecContext *avCodecContext=NULL;

public:
    HAudio();
    ~HAudio();
};


#endif //RESTUDYAV_HAUDIO_H
