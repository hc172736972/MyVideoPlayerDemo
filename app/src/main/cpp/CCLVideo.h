//
// Created by chencl on 21-5-11.
//

#ifndef CCLPLAYER_CCLVIDEO_H
#define CCLPLAYER_CCLVIDEO_H
#include "calljava/CCLJavaCall.h"
#include "CCLBasePlayer.h"
#include "CCLAudio.h"
#include "CCLPlayStatus.h"
#include "utils/CCLStataus.h"
class CCLVideo : public CCLBasePlayer{
public:
    CCLQueue *queue = NULL;
    CCLPlayStatus *cclPlayStatus;
    CCLJavaCall *cclJavaCall;
    CCLAudio *cclAudio;
    int codecType = -1;
    pthread_t videoThread;
    pthread_t decFrame;
    bool isExit = true;
    bool isExit2 = true;
    int playcount = -1;
    double delayTime = 0;
    double framePts = 0;
    double video_clock = 0;
public:
    CCLVideo(CCLJavaCall *javaCall,CCLAudio *audio,CCLPlayStatus *playStatus);
    ~CCLVideo();

    void playVideo(int codeType);
    void decodVideo();
    void release();
    double synchronize(AVFrame *srcFrame, double pts);

    double getDelayTime(double diff);

    void setClock(int secds);
};


#endif //CCLPLAYER_CCLVIDEO_H
