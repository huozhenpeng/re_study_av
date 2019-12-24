//
// Created by 霍振鹏 on 2019-12-24.
// 负责日志输出
//

#ifndef RESTUDYAV_LOG_H
#define RESTUDYAV_LOG_H
#



#endif //RESTUDYAV_LOG_H
#include <android/log.h>
//在jni中输出日志
#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"VoicePlayer",FORMAT,##__VA_ARGS__);