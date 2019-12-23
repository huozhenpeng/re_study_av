#include <jni.h>
#include <string>


#include <android/log.h>
//在jni中输出日志
#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"VoicePlayer",FORMAT,##__VA_ARGS__);


extern "C"
{
  #include <libavformat/avformat.h>
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_miduo_player_Player_stringFromJNI(JNIEnv *env, jobject thiz) {
    std::string hello = "Hello from C++";

    return env->NewStringUTF(hello.c_str());
}

/**
 * 打印ffmpeg支持的解码器
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_miduo_player_Player_printCodec(JNIEnv *env, jobject thiz) {
    av_register_all();
    AVCodec *codec=av_codec_next(NULL);
    while (codec!=NULL)
    {
        switch (codec->type)
        {
            case AVMEDIA_TYPE_VIDEO:
                LOGI("[Video]:%s",codec->name);
                break;
            case AVMEDIA_TYPE_AUDIO:
                LOGI("[Audio]:%s", codec->name);
                break;
            default:
                LOGI("[Other]:%s", codec->name);
                break;
        }
        codec=codec->next;
    }
}