//
// Created by chencl on 21-5-11.
//

#include "CCLJavaCall.h"
#include "../utils/AndroidLog.h"
#include "../utils/CCLStataus.h"


CCLJavaCall::CCLJavaCall(_JavaVM *vm, JNIEnv *env, jobject *jobj) {

    jniJavaVm = vm;
    jniEnv = env;
    jniJobj = *jobj;
    jniJobj = env->NewGlobalRef(jniJobj);
    jclass  jlz = jniEnv->GetObjectClass(jniJobj);
    if (!jlz) {
        if (LOG_IS_OPEN) {
           LOGE(" CCLJavaCall 初始化 jclass 失败 " );
        }
        return;
    }

     jmid_error = jniEnv->GetMethodID(jlz,"onError","(ILjava/lang/String;)V");
     jmid_olad = jniEnv->GetMethodID(jlz,"onLoad","(Z)V");
    jmid_isonlysoft = jniEnv->GetMethodID(jlz,"isOnlySoft","()Z");
    jmid_oninitmediacodec = jniEnv->GetMethodID(jlz,"mediacodecInit","(III[B[B)V");
    jmid_parpared = jniEnv->GetMethodID(jlz,"onParpared","()V");
    jmid_complete = jniEnv->GetMethodID(jlz,"videoComplete","()V");

}

void CCLJavaCall::onError(int type, int code, const char *msg) {
    if (type == WL_THREAD_CHILD) {
        JNIEnv *jniEnv;
        if (jniJavaVm->AttachCurrentThread(&jniEnv,0) != JNI_OK) {
            if (LOG_IS_OPEN) {
                LOGE(" onError 获取 AttachCurrentThread 失败 ");
            }
        }
        jstring jmsg = jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(jniJobj,jmid_error,code,jmsg);
        jniEnv->DeleteLocalRef(jmsg);
        jniJavaVm->DetachCurrentThread();
    } else if (type = WL_THREAD_MAIN) {
        jstring  jmsj = jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(jniJobj,jmid_error,code,jmsj);
        jniEnv->DeleteLocalRef(jmsj);
    } else {
        if (LOG_IS_OPEN) {
            LOGE(" onError 无效的线程id =  %d" , type);
        }
    }

}

void CCLJavaCall::onLoad(int type, bool isonlymusic) {
     if (type == WL_THREAD_CHILD) {
         JNIEnv *jniEnv ;
         if (jniJavaVm->AttachCurrentThread(&jniEnv,0) != JNI_OK) {
             if (LOG_IS_OPEN) {
                 LOGE(" onLoad 获取 AttachCurrentThread 失败");
             }
             return;
         }
         jniEnv->CallVoidMethod(jniJobj,jmid_olad,isonlymusic);
         jniJavaVm->DetachCurrentThread();
     } else if (type == WL_THREAD_MAIN)  {
        jniEnv->CallVoidMethod(jniJobj,jmid_olad,isonlymusic);
     } else {
         if (LOG_IS_OPEN) {
             LOGE(" onLoad 无效的线程id =  %d" , type);
         }
     }
}

bool CCLJavaCall::isOnlySoft(int type) {

    bool  soft = false;
    if (type == WL_THREAD_CHILD) {
        JNIEnv *jniEnv;
        if (jniJavaVm->AttachCurrentThread(&jniEnv,0) != JNI_OK) {
            if (LOG_IS_OPEN) {
                LOGE(" isOnlySoft 获取 isOnlySoft 失败");
            }
        }
        soft= jniEnv->CallBooleanMethod(jniJobj,jmid_isonlysoft);
        jniJavaVm->DetachCurrentThread();
    } else if (type == WL_THREAD_MAIN) {
        soft = jniEnv->CallBooleanMethod(jniJobj,jmid_isonlysoft);
    }
    return soft;
}

void CCLJavaCall::onInitMediacodec(int type, int mimetype, int width, int height, int csd_0_size,
                                   int csd_1_size, uint8_t *csd_0, uint8_t *csd_1) {
      if (type == WL_THREAD_CHILD) {

          JNIEnv *jniEnv;
          if(jniJavaVm->AttachCurrentThread(&jniEnv,0) != JNI_OK) {
              if (LOG_IS_OPEN) {
                  LOGE(" onInitMediacodec 获取  失败");
              }
              return;
          }

          jbyteArray  csd0 = jniEnv->NewByteArray(csd_0_size);
          jniEnv->SetByteArrayRegion(csd0,0,csd_0_size,(jbyte *)csd_0);

          jbyteArray  csd1 = jniEnv->NewByteArray(csd_1_size);
          jniEnv->SetByteArrayRegion(csd1, 0, csd_1_size, (jbyte *)csd_1);
          jniEnv->CallVoidMethod(jniJobj,jmid_oninitmediacodec,mimetype,width,height,csd0,csd1);

          jniEnv->DeleteLocalRef(csd0);
          jniEnv->DeleteLocalRef(csd1);
          jniJavaVm->DetachCurrentThread();

      } else if (type == WL_THREAD_MAIN) {
          jbyteArray  csd0 = jniEnv->NewByteArray(csd_0_size);
          jniEnv->SetByteArrayRegion(csd0,0,csd_0_size,(jbyte *)csd_0);

          jbyteArray  csd1 = jniEnv->NewByteArray(csd_1_size);
          jniEnv->SetByteArrayRegion(csd1, 0, csd_1_size, (jbyte *)csd_1);
          jniEnv->CallVoidMethod(jniJobj,jmid_oninitmediacodec,mimetype,width,height,csd0,csd1);

          jniEnv->DeleteLocalRef(csd0);
          jniEnv->DeleteLocalRef(csd1);
      }

}

void CCLJavaCall::onParpared(int type) {
    if (type == WL_THREAD_CHILD) {
        JNIEnv *jniEnv;
        if (jniJavaVm->AttachCurrentThread(&jniEnv,0) != JNI_OK) {
            if (LOG_IS_OPEN) {
                LOGE(" onParpared 获取  失败");
            }
            return;
        }
        jniEnv->CallVoidMethod(jniJobj,jmid_parpared);
        jniJavaVm->DetachCurrentThread();
    } else if (type = WL_THREAD_MAIN) {
        jniEnv->CallVoidMethod(jniJobj,jmid_parpared);
    }

}

void CCLJavaCall::onComplete(int type) {
    if (type == WL_THREAD_CHILD) {
        JNIEnv *jniEnv;
        if (jniJavaVm->AttachCurrentThread(&jniEnv,0) != JNI_OK) {
            if (LOG_IS_OPEN) {
                LOGE(" onComplete 获取  失败");
            }
            return;
        }
        jniEnv->CallVoidMethod(jniJobj,jmid_complete);
        jniJavaVm->DetachCurrentThread();
    } else if (type == WL_THREAD_MAIN) {
        jniEnv->CallVoidMethod(jniJobj,jmid_complete);
    }
}

void CCLJavaCall::onVideoInfo(int type, double currentTime, int totalTime) {

}

void CCLJavaCall::onDecMediacodec(int type, int size, uint8_t *data, int adata) {

}

void
CCLJavaCall::onGlRenderYuv(int type, int width, int height, uint8_t *y, uint8_t *u, uint8_t *v) {

}
