//
// Created by chencl on 21-5-11.
//

#ifndef CCLPLAYER_CCLJAVACALL_H
#define CCLPLAYER_CCLJAVACALL_H
#include <jni.h>

class CCLJavaCall {

public:
    _JavaVM  *jniJavaVm = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jniJobj;
    jmethodID jmid_error;
    jmethodID jmid_olad;
    jmethodID jmid_isonlysoft;
    jmethodID jmid_oninitmediacodec;
    jmethodID jmid_parpared;
    jmethodID jmid_complete;

public:
    CCLJavaCall(_JavaVM *vm, JNIEnv * env,jobject *jobj);
    ~CCLJavaCall();
    void onError(int type, int code ,const char *msg);
    void onLoad(int type, bool isonlymusic);
    bool isOnlySoft(int type);
    void onInitMediacodec(int type, int mimetype, int width, int height, int csd_0_size, int csd_1_size, uint8_t * csd_0, uint8_t * csd_1);
    void onParpared(int type);
    void onComplete(int type);

    void onVideoInfo(int type, double currentTime, int totalTime);

    void onDecMediacodec(int type, int size, uint8_t *data, int adata);
    void onGlRenderYuv(int type, int width, int height, uint8_t *y, uint8_t *u, uint8_t *v);
};


#endif //CCLPLAYER_CCLJAVACALL_H
