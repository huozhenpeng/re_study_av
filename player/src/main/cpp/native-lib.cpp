#include <jni.h>
#include <string>


extern "C"
JNIEXPORT jstring JNICALL
Java_com_miduo_player_Player_stringFromJNI(JNIEnv *env, jobject thiz) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}