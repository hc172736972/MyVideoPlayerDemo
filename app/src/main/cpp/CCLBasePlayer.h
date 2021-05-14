//
// Created by chencl on 21-5-11.
//

#ifndef CCLPLAYER_CCLBASEPLAYER_H
#define CCLPLAYER_CCLBASEPLAYER_H
#include "utils/AndroidLog.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/time.h>
#include <libswresample/swresample.h>
};

class CCLBasePlayer {

public:
    int  streamIndex;
    int sample_rate = 44100;
    int duration;
    int rate = 0;
    double clock = 0;
    double now_time = 0;
    bool  frameratebig = false;
    AVCodecContext  *avCodecContext = NULL;
    AVRational  time_base;
public:
    CCLBasePlayer();
    ~CCLBasePlayer();

};


#endif //CCLPLAYER_CCLBASEPLAYER_H
