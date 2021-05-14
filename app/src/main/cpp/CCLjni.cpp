#include <jni.h>
#include <string>
#include "utils/AndroidLog.h"
#include "calljava/CCLJavaCall.h"
#include "CCLFFmpeg.h"
#include "utils/CCLStataus.h"

_JavaVM *javaVM = NULL;
CCLJavaCall *cclJavaCall = NULL;
CCLFFmpeg *cclFFmpeg = NULL;


extern "C"  JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm,void * reserved) {
    jint result = -1;
    javaVM = vm;
    JNIEnv * env;

    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        if (LOG_IS_OPEN) {
            LOGE(" JNI_OnLoad 获取 JNIEnv 失败 ");
        }
        return result;
    }

    return JNI_VERSION_1_6;

}

extern "C" JNIEXPORT void JNICALL
Java_com_chencl_cclplayer_MainActivity_cclPrepared(JNIEnv *env, jobject thiz, jstring url_,
                                                  jboolean is_only_music) {
    const char * url = env->GetStringUTFChars(url_,0);

    //设置底层回调Java
    if (cclJavaCall == NULL) {
        cclJavaCall = new CCLJavaCall(javaVM,env,&thiz);
    }

    //设置初始化FFmPEG
    if (cclFFmpeg == NULL) {
        cclFFmpeg = new CCLFFmpeg(cclJavaCall,url,is_only_music);
        cclJavaCall->onLoad(WL_THREAD_MAIN, true);
        cclFFmpeg->preparedFFmpeg();
    }

}

extern "C"  JNIEXPORT void JNICALL
Java_com_chencl_cclplayer_MainActivity_cclStart(JNIEnv *env, jobject thiz) {
    if (cclFFmpeg != NULL) {
        cclFFmpeg->start();
    }
}