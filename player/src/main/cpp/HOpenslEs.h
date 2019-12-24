//
// Created by 霍振鹏 on 2019-12-24.
//

#ifndef RESTUDYAV_HOPENSLES_H
#define RESTUDYAV_HOPENSLES_H

#include "HAudio.h"

extern "C"
{
#include <string>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
}

class HOpenslEs
{
public:
    void initOpenSLES();
    HOpenslEs(HAudio *hAudio,int sample_rate);
    ~HOpenslEs();

public:
    int sample_rate;
    SLObjectItf  engineObject;
    SLEngineItf  engineEngine;
    SLObjectItf outputMixObject;

    SLEnvironmentalReverbItf  outputMixEnvironmentalReverb;
    SLEnvironmentalReverbSettings reverbSettings;

    SLObjectItf pcmPlayerObject;

    SLPlayItf  pcmPlayerPlay;

    SLAndroidSimpleBufferQueueItf pcmBufferQueue;

    HAudio *hAudio;

};
#endif //RESTUDYAV_HOPENSLES_H
