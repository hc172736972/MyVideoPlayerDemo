//
// Created by chencl on 21-5-11.
//

#include "CCLFFmpeg.h"
#include "utils/CCLStataus.h"

void *decodeTThread(void *data) {
    CCLFFmpeg *cclfFmpeg = (CCLFFmpeg *)data;
    cclfFmpeg->decodeFFmpeg();
    pthread_exit(&cclfFmpeg->decodeThread);

}

CCLFFmpeg::CCLFFmpeg(CCLJavaCall *javaCall, const char *url, bool isonlymusic) {
   pthread_mutex_init(&init_mutex,NULL);
   pthread_mutex_init(&seek_mutex,NULL);
   exitByUser = false;
   isOnlyMusic = isonlymusic;
   cclJavaCall = javaCall;
   urlPath = url;
   cclPlayStatus = new CCLPlayStatus();

}

int avformat_interrupt_cb(void *ctx) {
    CCLFFmpeg *cclfFmpeg = (CCLFFmpeg *)ctx;
    if (cclfFmpeg->cclPlayStatus->exit) {
        if (LOG_IS_OPEN) {
            LOGE(" avformat_interrupt_cb return 1 ");
        }
        return AVERROR_EOF;
    }
    if (LOG_IS_OPEN) {
        LOGE("avformat_interrupt_cb return 0");
    }
    return 0;
}

int CCLFFmpeg::preparedFFmpeg() {
    pthread_create(&decodeThread,NULL,decodeTThread,this);
    return 0;
}

int CCLFFmpeg::decodeFFmpeg() {
    pthread_mutex_lock(&init_mutex);

    exit = false;
    isavi = false;
    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();
    if (avformat_open_input(&pFormatCtx,urlPath,NULL,NULL) != 0) {
        if (LOG_IS_OPEN) {
            LOGE(" 不能打开文件 %s ",urlPath);
        }
        if (cclJavaCall != NULL) {
            cclJavaCall->onError(WL_THREAD_CHILD,WL_FFMPEG_CAN_NOT_OPEN_URL,"can not open url");
        }
        exit = true;
        pthread_mutex_unlock(&init_mutex);
        return -1;
    }
    pFormatCtx->interrupt_callback.callback = avformat_interrupt_cb;
    pFormatCtx->interrupt_callback.opaque = this;
    if (avformat_find_stream_info(pFormatCtx,NULL) < 0) {
        if (LOG_IS_OPEN) {
            LOGE(" 不能读取到流 %s" ,urlPath);
        }
        if (cclJavaCall != NULL) {
            cclJavaCall->onError(WL_THREAD_CHILD,WL_FFMPEG_CAN_NOT_FIND_STREAMS,"can not find streams from url");
        }
        exit = true;
        pthread_mutex_unlock(&init_mutex);
        return  -1;
    }

    if (pFormatCtx == NULL) {
        exit = true;
        pthread_mutex_unlock(&init_mutex);
        if (LOG_IS_OPEN) {
            LOGE(" pFormatCtx 为空！");
        }
        return -1;
    }

    duration = pFormatCtx->duration / 1000000;
    if (LOG_IS_OPEN) {
        LOGD(" 通道个数为 %d" ,pFormatCtx->nb_streams);
    }

    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) //音频
        {
            if (LOG_IS_OPEN) {
                LOGD(" 音频数据");
            }
            CCLAudioChannel *cl = new CCLAudioChannel(i,pFormatCtx->streams[i]->time_base);
            audiochannels.push_front(cl);
        }
        else if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) //视频
        {
            if (!isOnlyMusic) {
                if (LOG_IS_OPEN) {
                    LOGE("视频数据");
                }
                int  num = pFormatCtx->streams[i]->avg_frame_rate.num;
                int  den = pFormatCtx->streams[i]->avg_frame_rate.den;
                if (num != 0 && den != 0) {
                    int fps = num / den;
                    CCLAudioChannel *cl = new CCLAudioChannel(i,pFormatCtx->streams[i]->time_base,fps);
                    videochannels.push_front(cl);
                }
            }
        }
        if (audiochannels.size() > 0) {
            cclAudio = new CCLAudio(cclPlayStatus,cclJavaCall);
            setAudioChannel(0);
            if (cclAudio->streamIndex >= 0 && cclAudio->streamIndex < pFormatCtx->nb_streams) {
                if (getAvCodecContext(pFormatCtx->streams[cclAudio->streamIndex]->codecpar,cclAudio) != 0) {
                    exit = true;
                    pthread_mutex_unlock(&init_mutex);
                    return  1;
                }
            }
        }


        if (videochannels.size() > 0) {
            cclVideo = new CCLVideo(cclJavaCall,cclAudio,cclPlayStatus);
            setVideoChannel(0);
            if (cclVideo->streamIndex >=0 && cclVideo->streamIndex < pFormatCtx->nb_streams) {
                if (getAvCodecContext(pFormatCtx->streams[cclVideo->streamIndex]->codecpar,cclVideo) != 0) {
                    exit = true;
                    pthread_mutex_unlock(&init_mutex);
                    return 1;
                }
            }
        }

        if (cclAudio == NULL && cclVideo == NULL) {
            exit = true;
            pthread_mutex_unlock(&init_mutex);
            return 1;
        }
        if (cclAudio != NULL) {
            cclAudio->duration = pFormatCtx-> duration / 1000000;
            cclAudio->sample_rate = cclAudio->avCodecContext->sample_rate;
            if (cclVideo!= NULL) {
                cclAudio->setVideo(true);
            }
        }
        if (cclVideo != NULL) {
            if (LOG_IS_OPEN) {
                LOGE("codec name is %s", cclVideo->avCodecContext->codec->name);
                LOGE("codec long name is %s", cclVideo->avCodecContext->codec->long_name);
            }
            if (!cclJavaCall->isOnlySoft(WL_THREAD_CHILD)) {
                mimeType = getMimeType(cclVideo->avCodecContext->codec->name);
            } else{
                mimeType = -1;
            }

            if (mimeType != -1) {
                cclJavaCall->onInitMediacodec(
                        WL_THREAD_CHILD,
                        mimeType,
                        cclVideo->avCodecContext->width,
                        cclVideo->avCodecContext->height,
                        cclVideo->avCodecContext->extradata_size,
                        cclVideo->avCodecContext->extradata_size,
                        cclVideo->avCodecContext->extradata,
                        cclVideo->avCodecContext->extradata);
            }
            cclVideo->duration = pFormatCtx ->duration /1000000;

        }

    }

    if (LOG_IS_OPEN) {
        LOGD("准备中。。。。");
    }
    cclJavaCall->onParpared(WL_THREAD_CHILD);
    if (LOG_IS_OPEN)
    {
        LOGD("准备完成。。。。");
    }
    exit = true;
    pthread_mutex_unlock(&init_mutex);
    return 0;
}

int CCLFFmpeg::start() {


    exit = false;
    int  count = 0;
    int ret = -1;
    if (cclAudio != NULL) {
        cclAudio->playAudio();
    }
    if (cclVideo != NULL) {
        if (mimeType == -1) {
            cclVideo->playVideo(0);
        } else {
            cclVideo->playVideo(1);
        }
    }


    AVBitStreamFilterContext *mimType = NULL;
    if (mimeType == 1) {
        mimType = av_bitstream_filter_init("h264_mp4toannexb");
    } else if (mimeType == 2) {
        mimType = av_bitstream_filter_init("hevc_mp4toannexb");
    }else if (mimeType == 3) {
        mimType = av_bitstream_filter_init("h264_mp4toannexb");
    } else if (mimeType == 4) {
        mimType = av_bitstream_filter_init("h264_mp4toannexb");
    }

    while(!cclPlayStatus->exit) {
        exit = false;
        if (cclPlayStatus->pause) { //暂停
            av_usleep(1000 * 100);
            continue;
        }

        if (cclAudio != NULL && cclAudio->queue->getAvPacketSize() > 100) {
            if (LOG_IS_OPEN) {
                LOGD(" cclAudio  getAvPacketSize  > 100");
            }
            av_usleep(1000 * 100);
            continue;
        }
        if (cclVideo != NULL && cclVideo->queue->getAvPacketSize() > 100) {
            if (LOG_IS_OPEN) {
                LOGD(" cclVideo  getAvPacketSize  > 100");
            }
            av_usleep(1000 * 100);
            continue;
        }

        AVPacket *packet = av_packet_alloc();
        pthread_mutex_lock(&seek_mutex);
        ret = av_read_frame(pFormatCtx,packet);
        pthread_mutex_unlock(&seek_mutex);
        if(cclPlayStatus->seek) {
            av_packet_free(&packet);
            av_free(packet);
            continue;
        }
        if (ret == 0) {
            if (cclAudio != NULL && packet->stream_index == cclAudio->streamIndex) {
                count ++;
                if (LOG_IS_OPEN) {
                    LOGD("解码第 %d 帧",count);
                }
                cclAudio->queue->putAvpacket(packet);
            } else if (cclVideo != NULL && packet ->stream_index == cclVideo->streamIndex) {
                if (mimeType != NULL && !isavi) {
                    uint8_t *data;
                    av_bitstream_filter_filter(mimType,pFormatCtx->streams[cclVideo->streamIndex]->codec,NULL,
                            &data,&packet->size,packet->data,packet->size,0);
                    uint8_t *tdata = NULL;
                    tdata = packet->data;
                    packet->data = data;
                    if (tdata != NULL) {
                        av_freep(tdata);
                    }
                }
                cclVideo->queue->putAvpacket(packet);
            } else{
                av_packet_free(&packet);
                av_freep(packet);
                packet = NULL;
            }
        } else{
            av_packet_free(&packet);
            av_freep(packet);
            packet = NULL;
            if ((cclVideo != NULL && cclVideo->queue->getAvPacketSize() == 0)||(cclAudio != NULL && cclAudio->queue->getAvPacketSize() == 0)){
                cclPlayStatus->exit = true;
                break;
            }
        }
    }
    if (mimeType != NULL) {
        av_bitstream_filter_close(mimType);
    }
    if (!exitByUser && cclJavaCall != NULL) {
        cclJavaCall->onComplete(WL_THREAD_CHILD);
    }
    exit = true;
    return 0;
}

int CCLFFmpeg::seek(ino64_t sec) {

    if (sec >= duration)
    {
        return  -1;
    }

    if (cclPlayStatus->load)
    {
        return -1;
    }

    if (pFormatCtx != NULL)
    {
        cclPlayStatus->seek = true;
        pthread_mutex_lock(&seek_mutex);
        int64_t  rel = sec * AV_TIME_BASE;
        int ret = avformat_seek_file(pFormatCtx,-1,INT64_MIN,rel,INT64_MAX,0);
        if (cclAudio != NULL) {
            cclAudio->queue->clearAvpacket();
            cclAudio->setClock(0);
        }
        if (cclVideo != NULL) {
            cclVideo->queue->clearAvFrame();
            cclVideo->queue->clearAvpacket();
            cclVideo->setClock(0);
        }

        cclAudio->clock = 0;
        cclAudio->now_time = 0;
        pthread_mutex_unlock(&seek_mutex);
        cclPlayStatus->seek = false;
    }
    return 0;
}

int CCLFFmpeg::getDuration() {
    return duration;
}

int CCLFFmpeg::getAvCodecContext(AVCodecParameters *parameters, CCLBasePlayer *cclBasePlayer) {

    AVCodec  *dec = avcodec_find_decoder(parameters->codec_id);
    if (!dec)
    {
        cclJavaCall->onError(WL_THREAD_CHILD,3," get avcodec fail");
        exit = true;
        return 1;
    }

    cclBasePlayer->avCodecContext = avcodec_alloc_context3(dec);
    if (!cclBasePlayer->avCodecContext) {
        cclJavaCall->onError(WL_THREAD_CHILD,4,"alloc avcodecctx fail");
    exit = true;
        return 1;
    }
    if (avcodec_open2(cclBasePlayer->avCodecContext,dec,0) != 0) {
        cclJavaCall->onError(WL_THREAD_CHILD,6,"open avcodecctx fail");
        exit = true;
        return 1;
    }

    return 0;
}

void CCLFFmpeg::release() {
    cclPlayStatus->exit = true;
    pthread_mutex_lock(&init_mutex);

    if (LOG_IS_OPEN)
    {
        LOGD(" 开始释放 CCLFFmpeg::release ");
    }

    int sleepCount = 0;
    while (!exit) {
        if (sleepCount > 1000)
        {
            exit = true;
        }
        if (LOG_IS_OPEN)
        {
            LOGD(" wait ffmpeg exit %d",sleepCount);
        }

        sleepCount ++ ;
        av_usleep(1000 * 10);
    }

    if (LOG_IS_OPEN)
    {
        LOGD("释放audio....................................");
    }


    if (cclAudio != NULL)
    {
        if (LOG_IS_OPEN)
        {
            LOGD("释放audio....................................2");
        }
        cclAudio->realease();
        delete (cclAudio);
        cclAudio = NULL;
    }
    if (LOG_IS_OPEN)
    {
        LOGD("释放video....................................");
    }

    if (cclVideo != NULL)
    {
        if (LOG_IS_OPEN)
        {
            LOGD("释放video....................................1");
        }

        cclVideo->release();
        delete (cclVideo);
        cclVideo = NULL;
    }

    if (LOG_IS_OPEN)
    {
        LOGD("释放javacall....................................");
    }

    if (cclJavaCall != NULL) {
        cclJavaCall = NULL;
    }
    pthread_mutex_unlock(&init_mutex);


}

void CCLFFmpeg::pause() {

    if (cclPlayStatus != NULL)
    {
        cclPlayStatus->pause = true;
        if (cclAudio != NULL)
        {
            cclAudio->pause();
        }
    }

}

void CCLFFmpeg::resume() {

    if (cclPlayStatus != NULL) {
        cclPlayStatus->pause = false;
        if (cclAudio != NULL)
        {
          cclAudio->resume();
        }
    }
}

int CCLFFmpeg::getMimeType(const char *codecName) {
    return 0;
}

void CCLFFmpeg::setAudioChannel(int id) {
    if (cclAudio != NULL) {
        int channelsize = audiochannels.size();
        if (id < channelsize) {
            for (int i = 0; i < channelsize; ++i) {
                if (i == id) {
                    cclAudio ->time_base = audiochannels.at(i)->time_base;
                    cclAudio->streamIndex = audiochannels.at(i)->channelId;
                }
            }
        }
    }

}

void CCLFFmpeg::setVideoChannel(int id) {
    if (cclVideo != NULL) {
        cclVideo->streamIndex = videochannels.at(id)->channelId;
        cclVideo->time_base = videochannels.at(id)->time_base;
        cclVideo->rate = 1000/ videochannels.at(id)->fps;
        if (videochannels.at(id)->fps >= 60) {
            cclVideo->frameratebig = true;
        } else {
            cclVideo->frameratebig = false;
        }

    }
}

int CCLFFmpeg::getAudioChannels() {
    return audiochannels.size();
}

int CCLFFmpeg::getVideoWidth() {

    if (cclVideo != NULL && cclVideo->avCodecContext != NULL)
    {
        return cclVideo->avCodecContext->width;
    }
    return 0;
}

int CCLFFmpeg::getVideoHeight() {
    if (cclVideo != NULL && cclVideo->avCodecContext != NULL)
    {
        return cclVideo->avCodecContext->width;
    }
    return 0;
}

CCLFFmpeg::~CCLFFmpeg() {
  pthread_mutex_destroy(&init_mutex);
  if (LOG_IS_OPEN) {
      LOGD(" ~CCLFFmpeg 释放了");
  }
}
