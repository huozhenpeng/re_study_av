//
// Created by 霍振鹏 on 2019-12-24.
//

#ifndef RESTUDYAV_CALLBACKJAVA_H
#define RESTUDYAV_CALLBACKJAVA_H


#include <jni.h>
#include <string>

#define MAIN_THREAD 1
#define CHILD_THREAD 0

class CallBackJava {


public:
    JavaVM *javaVm=NULL;
    JNIEnv *jniEnv=NULL;
    jobject instance=NULL;
    jmethodID jmd=NULL;

public:
    CallBackJava(JavaVM *vm,JNIEnv *env,jobject job);
    ~CallBackJava();


    /**
     * 回调
     * @param type
     * @param code
     * @param msg
     */
    void onCallBack(int type,int code, const char *msg);


};


#endif //RESTUDYAV_CALLBACKJAVA_H
