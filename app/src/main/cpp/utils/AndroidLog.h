//
// Created by chencl on 21-5-11.
//

#ifndef CCLPLAYER_ANDROIDLOG_H
#define CCLPLAYER_ANDROIDLOG_H
#include <android/log.h>
#define LOG_IS_OPEN true

#define LOGD(FORMAT,...) __android_log_print(ANDROID_LOG_DEBUG,"ywl5320",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"ywl5320",FORMAT,##__VA_ARGS__);

#endif //CCLPLAYER_ANDROIDLOG_H
