//
// Created by chencl on 21-5-11.
//

#ifndef CCLPLAYER_CCLAUDIOCHANNEL_H
#define CCLPLAYER_CCLAUDIOCHANNEL_H
extern "C" {
#include <libavutil/rational.h>
};



class CCLAudioChannel {
public:
    int channelId = -1;
    AVRational time_base;
    int fps;
public:
    CCLAudioChannel(int id,AVRational base);
    CCLAudioChannel(int id,AVRational base,int fps);
    ~CCLAudioChannel();
};


#endif //CCLPLAYER_CCLAUDIOCHANNEL_H
