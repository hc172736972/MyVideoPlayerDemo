//
// Created by chencl on 21-5-11.
//

#ifndef CCLPLAYER_CCLAUDIO_H
#define CCLPLAYER_CCLAUDIO_H
#include "CCLBasePlayer.h"
#include "CCLPlayStatus.h"
#include "calljava/CCLJavaCall.h"
#include "CCLQueue.h"
#include "pthread.h"


class CCLAudio : public CCLBasePlayer{

public:
    int ret = 0;//函数调用返回结果
    pthread_t  audioThread;
    CCLQueue *queue = NULL;
    bool isExit = false;
    bool  isVideo = false;
    bool isReadPacketFinish = true;
    CCLPlayStatus *cclPlayStatus = NULL;
    CCLJavaCall *cclJavaCall = NULL;
    AVPacket *packet;
    void *buffer = NULL;
    int pcmsize = 0;
    // 引擎接口
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine = NULL;

    int64_t dst_layout = 0;//重采样为立体声
    int dst_nb_samples = 0;// 计算转换后的sample个数 a * b / c
    int nb = 0;//转换，返回值为转换后的sample个数
    uint8_t *out_buffer = NULL;//buffer 内存区域
    int out_channels = 0;//输出声道数
    int data_size = 0;//buffer大小
    enum AVSampleFormat dst_format;


    //混音器
    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

    //pcm
    SLObjectItf pcmPlayerObject = NULL;
    SLPlayItf pcmPlayerPlay = NULL;
    SLVolumeItf pcmPlayerVolume = NULL;

    //缓冲器队列接口
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;

public:
    CCLAudio(CCLPlayStatus *playStatus, CCLJavaCall *javaCall);
    ~CCLAudio();
    void setVideo(bool video);


    void playAudio();

    int initOPenSL();

    int getSLSampleRate();

    int getPcmData(void **pcm);

    void pause();
    void resume();
    void realease();
    void setClock(int secds);
};


#endif //CCLPLAYER_CCLAUDIO_H
