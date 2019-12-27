//
// Created by 霍振鹏 on 2019-12-24.
//

#include "CallBackJava.h"
#include "../head/log.h"

CallBackJava::CallBackJava(JavaVM *vm, JNIEnv *env, jobject job) {

    javaVm=vm;
    jniEnv=env;
    instance=job;
    jclass jcl=env->GetObjectClass(job);
    //现在的androidstudio好智能，java端不写这个方法也会报错
    jmd=env->GetMethodID(jcl,"callBackJava","(ILjava/lang/String;)V");
    jmd_complete=env->GetMethodID(jcl,"onPlayComplete","(ILjava/lang/String;)V");
    jmd_time=env->GetMethodID(jcl,"onShowTime","(III)V");
    jmid_valumedb = env->GetMethodID(jcl, "onCallValumeDB", "(II)V");
}

void CallBackJava::onCallBack(int type, int code, const char *msg) {

    if(type==CHILD_THREAD)
    {
        //重新给jniEnv赋值
        javaVm->AttachCurrentThread(&jniEnv,0);
        //构造返回给java的字符串
        jstring  jsr=jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(instance,jmd,code,jsr);
        //释放jstring
        jniEnv->DeleteLocalRef(jsr);
        javaVm->DetachCurrentThread();
    } else if(type==MAIN_THREAD)
    {
        jstring  jsr=jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(instance,jmd,code,jsr);
        //释放jstring
        jniEnv->DeleteLocalRef(jsr);
    }
}

CallBackJava::~CallBackJava() {

    LOGI("CallBackJava 析构函数执行了");

}

void CallBackJava::onShowTime(int type, int code, int total, int current) {
    if(type==CHILD_THREAD)
    {
        //重新给jniEnv赋值
        javaVm->AttachCurrentThread(&jniEnv,0);

        jniEnv->CallVoidMethod(instance,jmd_time,code,total,current);

        javaVm->DetachCurrentThread();
    } else if(type==MAIN_THREAD)
    {
        jniEnv->CallVoidMethod(instance,jmd_time,code,total,current);
    }
}


void CallBackJava::onPlayComplete(int type, int code, const char *msg) {

    if(type==CHILD_THREAD)
    {
        //重新给jniEnv赋值
        javaVm->AttachCurrentThread(&jniEnv,0);
        //构造返回给java的字符串
        jstring  jsr=jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(instance,jmd_complete,code,jsr);
        //释放jstring
        jniEnv->DeleteLocalRef(jsr);
        javaVm->DetachCurrentThread();
    } else if(type==MAIN_THREAD)
    {
        jstring  jsr=jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(instance,jmd_complete,code,jsr);
        javaVm->DetachCurrentThread();
    }
}

void CallBackJava::onCallValumeDB(int type, int db,int currentTime) {
    if(type == MAIN_THREAD)
    {
        jniEnv->CallVoidMethod(instance, jmid_valumedb, db,currentTime);
    }
    else if(type == CHILD_THREAD)
    {
        //重新给jniEnv赋值
        javaVm->AttachCurrentThread(&jniEnv,0);
        //构造返回给java的字符串
        jniEnv->CallVoidMethod(instance,jmid_valumedb,db,currentTime);
        javaVm->DetachCurrentThread();
    }
}

