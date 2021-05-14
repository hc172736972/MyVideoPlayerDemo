//
// Created by chencl on 21-5-11.
//

#ifndef CCLPLAYER_CCLFFMPEG_H
#define CCLPLAYER_CCLFFMPEG_H
#include "deque"
#include "calljava/CCLJavaCall.h"
#include "pthread.h"
#include "utils/AndroidLog.h"
#include "CCLAudio.h"
#include "CCLVideo.h"
#include "CCLPlayStatus.h"
#include "CCLAudioChannel.h"
#include "CCLBasePlayer.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/time.h>
}

class CCLFFmpeg {
public:
    const char *urlPath = NULL;
    CCLJavaCall *cclJavaCall = NULL;
    pthread_t decodeThread;
    AVFormatContext *pFormatCtx = NULL;
    CCLVideo *cclVideo = NULL;
    CCLAudio *cclAudio = NULL;
    CCLPlayStatus *cclPlayStatus = NULL;
    int duration = 0;
    bool exit = false;
    bool exitByUser = false;
    bool mimeType = 1;
    bool isavi = false;
    bool isOnlyMusic = false;
    std::deque<CCLAudioChannel*> audiochannels;
    std::deque<CCLAudioChannel*> videochannels;
    pthread_mutex_t init_mutex;
    pthread_mutex_t seek_mutex;

public:
    CCLFFmpeg(CCLJavaCall *javaCall,const char *url, bool isonlymusic);
    ~CCLFFmpeg();
    int preparedFFmpeg();
    int decodeFFmpeg();
    int start();
    int seek(ino64_t sec);
    int getDuration();
    int getAvCodecContext(AVCodecParameters *parameters,CCLBasePlayer *cclBasePlayer);
    void release();
    void pause();
    void resume();
    int getMimeType(const char * codecName);
    void setAudioChannel(int id);
    void setVideoChannel(int id);
    int getAudioChannels();
    int getVideoWidth();
    int getVideoHeight();

};


#endif //CCLPLAYER_CCLFFMPEG_H
