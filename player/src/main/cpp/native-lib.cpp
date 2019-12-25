#include <jni.h>
#include <string>

#include "head/log.h"
#include "HFFmpeg.h"


extern "C"
{
    //CMakeLists中配置了include_directories(src/main/cpp/include/ffmpeg)，所以可以这么写
  #include <libavformat/avformat.h>
}



JNIEXPORT void JNICALL
JNI_OnUnload(JavaVM *vm,void* reserved)
{

}

JavaVM *javaVm;
/**
 * 主要是获取JavaVM，供子线程构建JNIEnv使用
 * @param vm
 * @param reserved
 * @return
 */
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm,void* reserved)
{
    JNIEnv *env;
    javaVm=vm;
    if(vm->GetEnv((void **)(&env), JNI_VERSION_1_6) != JNI_OK)
    {
        return -1;
    }
    return JNI_VERSION_1_6;
}


HFFmpeg *hFFmpeg=NULL;
CallBackJava *callBackJava=NULL;
HPlayStatus *hPlayStatus=NULL;

extern "C"
JNIEXPORT void JNICALL
Java_com_miduo_player_Player_prepare(JNIEnv *env, jobject thiz, jstring url) {

    const  char* path=env->GetStringUTFChars(url,0);
    //需要优化不
    //这个path在哪释放啊

    if(hFFmpeg==NULL)
    {
        if(callBackJava==NULL)
        {
            callBackJava=new CallBackJava(javaVm,env,env->NewGlobalRef(thiz));
            //需要释放不
            //在哪儿释放这个全局引用啊
        }
        hPlayStatus=new HPlayStatus();
        hFFmpeg=new HFFmpeg(path,callBackJava,hPlayStatus);
        hFFmpeg->prepare();
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_miduo_player_Player_start(JNIEnv *env, jobject thiz) {
    hFFmpeg->start();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_miduo_player_Player_pause(JNIEnv *env, jobject thiz) {
    hFFmpeg->pause();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_miduo_player_Player_resume(JNIEnv *env, jobject thiz) {
    hFFmpeg->resume();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_miduo_player_Player_stop(JNIEnv *env, jobject thiz) {
    hFFmpeg->stop();
    hFFmpeg->release();
    if(callBackJava!=NULL)
    {
        delete(callBackJava);
        callBackJava=NULL;
    }
    if(hPlayStatus!=NULL)
    {
        delete(hPlayStatus);
        hPlayStatus=NULL;
    }
    if(hFFmpeg!=NULL)
    {
        delete(hFFmpeg);
        hFFmpeg=NULL;
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_miduo_player_Player_seek(JNIEnv *env, jobject thiz, jint second) {
    hFFmpeg->seek(second);
}