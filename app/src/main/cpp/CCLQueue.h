//
// Created by chencl on 21-5-12.
//

#ifndef CCLPLAYER_CCLQUEUE_H
#define CCLPLAYER_CCLQUEUE_H


#include "CCLPlayStatus.h"
#include "queue"
#include "utils/AndroidLog.h"
extern "C" {
#include "pthread.h"
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
};
class CCLQueue {
public:
    std::queue<AVPacket*> queuePacket;
    std::queue<AVFrame*> queueFrame;
    pthread_mutex_t mutexFrame;
    pthread_cond_t condFrame;
    pthread_mutex_t mutexPacket;
    pthread_cond_t condPacket;
    CCLPlayStatus *cclPlayStatus = NULL;


public:
    CCLQueue(CCLPlayStatus *playStatus);
    ~CCLQueue();
    int getAvPacketSize();
    int getAvFrameSize();
    int putAvpacket(AVPacket *avPacket);
    int getAvpacket(AVPacket *avPacket);
    int clearAvpacket();
    int noticeThread();

    void release();
    int clearAvFrame();
    int putAvframe(AVFrame *pFrame);

    int getAvframe(AVFrame *pFrame);

    int clearToKeyFrame();
};


#endif //CCLPLAYER_CCLQUEUE_H
