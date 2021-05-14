//
// Created by chencl on 21-5-11.
//

#include "CCLVideo.h"

CCLVideo::~CCLVideo() {

}


void * decodvideoT(void *data) {
    CCLVideo *cclVideo = (CCLVideo *)data;
    cclVideo->decodVideo();
    pthread_exit(&cclVideo->videoThread);
}


void * codecFrame(void *data) {
    CCLVideo *cclVideo = (CCLVideo *)data;
    while (!cclVideo->cclPlayStatus->exit) {
            if (cclVideo->cclPlayStatus->seek) {
                continue;
            }
            cclVideo->isExit2= false;
            if (cclVideo->queue->getAvPacketSize() > 20) {
                continue;
            }

            if (cclVideo->codecType == 1)
            {
                if(cclVideo->queue->getAvPacketSize() == 0)//加载
                {
                    if (!cclVideo->cclPlayStatus->load) {
                        cclVideo->cclJavaCall->onLoad(WL_THREAD_CHILD, true);
                        cclVideo->cclPlayStatus->load = true;
                    }
                    continue;
                }else {
                    if (cclVideo->cclPlayStatus->load) {
                        cclVideo->cclJavaCall->onLoad(WL_THREAD_CHILD, false);
                        cclVideo->cclPlayStatus->load = false;
                    }
                }
            }

            AVPacket *packet = av_packet_alloc();
            if (cclVideo->queue->getAvpacket(packet) != 0) {
                av_packet_free(&packet);
                av_free(packet);
                packet = NULL;
                continue;
            }

            int ret = avcodec_send_packet(cclVideo->avCodecContext,packet);
            if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
            {
                av_packet_free(&packet);
                av_free(packet);
                packet = NULL;
                continue;
            }
            AVFrame  *frame = av_frame_alloc();
            ret = avcodec_receive_frame(cclVideo->avCodecContext,frame);

            if (ret < 0 && ret != AVERROR_EOF)
            {
                av_frame_free(&frame);
                av_free(frame);
                frame = NULL;
                av_packet_free(&packet);
                av_free(packet);
                packet = NULL;
                continue;
            }
            cclVideo->queue->putAvframe(frame);
            av_packet_free(&packet);
            av_free(packet);
            packet = NULL;
    }
    cclVideo->isExit2 = true;
    pthread_exit(&cclVideo->decFrame);
}

CCLVideo::CCLVideo(CCLJavaCall *javaCall, CCLAudio *audio, CCLPlayStatus *playStatus) {
    cclAudio = audio;
    cclJavaCall = javaCall;
    cclPlayStatus = playStatus;
    queue = new CCLQueue(cclPlayStatus);

}

void CCLVideo::playVideo(int type) {

    codecType = type;
    if (codecType == 0) {
        pthread_create(&decFrame,NULL,codecFrame, this);
    }
    pthread_create(&videoThread,NULL,decodvideoT, this);

}

void CCLVideo::decodVideo() {
    while (!cclPlayStatus->exit)
    {
        isExit = false;
        if (cclPlayStatus->pause)
        {
            continue;
        }
        if (cclPlayStatus->seek)
        {
            cclJavaCall->onLoad(WL_THREAD_CHILD, true);
            cclPlayStatus->load = true;
            continue;
        }

        if (queue->getAvPacketSize() == 0)
        {
            if (!cclPlayStatus->load)
            {
                cclJavaCall->onLoad(WL_THREAD_CHILD, true);
                cclPlayStatus->load = true;
            }
            continue;
        } else
        {
            if (cclPlayStatus->load)
            {
                cclJavaCall->onLoad(WL_THREAD_CHILD, false);
                cclPlayStatus->load = false;
            }
        }

        if (codecType == 1)
        {
            AVPacket *packet = av_packet_alloc();
            if (queue->getAvpacket(packet) != 0)
            {
                av_free(packet->data);
                av_free(packet->buf);
                av_free(packet->side_data);
                packet = NULL;
                continue;
            }
            double time = packet->pts * av_q2d(time_base);
            if(LOG_IS_OPEN)
            {
                LOGE("video clock is %f", time);
                LOGE("audio clock is %f", cclAudio->clock);
            }

            if (time < 0)
            {
                time = packet->dts * av_q2d(time_base);
            }

            if (time < clock)
            {
                time = clock;
            }
            clock = time;
            double diff = 0;
            if (cclAudio != NULL)
            {
                diff = cclAudio->clock - clock;
            }
            playcount++;
            if (playcount > 500)
            {
                playcount = 0;
            }
            if (diff >= 0.5)
            {
                if (frameratebig)
                {
                    if (playcount % 3  == 0 && packet->flags != AV_PKT_FLAG_KEY)
                    {
                        av_free(packet->data);
                        av_free(packet->buf);
                        av_free(packet->side_data);
                        packet = NULL;
                        continue;
                    }
                } else
                    {
                        av_free(packet->data);
                        av_free(packet->buf);
                        av_free(packet->side_data);
                        packet = NULL;
                        continue;
                    }
            }

            delayTime = getDelayTime(diff);
            if(LOG_IS_OPEN)
            {
                LOGE("delay time %f diff is %f", delayTime, diff);
            }

            av_usleep(delayTime * 1000);
            cclJavaCall->onVideoInfo(WL_THREAD_CHILD, clock, duration);
            cclJavaCall->onDecMediacodec(WL_THREAD_CHILD, packet->size, packet->data, 0);
            av_free(packet->data);
            av_free(packet->buf);
            av_free(packet->side_data);
            packet = NULL;
        }
        else if (codecType == 0)
        {
            AVFrame * frame = av_frame_alloc();
            if (queue->getAvframe(frame) != 0)
            {
                av_frame_free(&frame);
                av_free(frame);
                frame = NULL;
                continue;
            }
            if ((framePts = av_frame_get_best_effort_timestamp(frame)) ==AV_NOPTS_VALUE)
            {
                framePts = 0;
            }
            framePts *= av_q2d(time_base);
            clock = synchronize(frame,framePts);
            double diff = 0;
            if (cclAudio != NULL)
            {
                diff = cclAudio->clock-clock;
            }
            delayTime = getDelayTime(diff);
            if(LOG_IS_OPEN)
            {
                LOGE("delay time %f diff is %f", delayTime, diff);
            }

            playcount++;
            if (playcount > 500)
            {
                playcount = 0;
            }
            if (diff >= 0.5)
            {
                if (frameratebig)
                {
                    if (playcount % 3 == 0)
                    {
                        av_frame_free(&frame);
                        av_free(frame);
                        frame = NULL;
                        queue->clearToKeyFrame();
                        continue;
                    }
                } else
                    {
                        av_frame_free(&frame);
                        av_free(frame);
                        frame = NULL;
                        queue->clearToKeyFrame();
                        continue;
                    }
            }

            av_usleep(delayTime * 1000);
            cclJavaCall->onVideoInfo(WL_THREAD_CHILD,clock,duration);
            cclJavaCall->onGlRenderYuv(WL_THREAD_CHILD, frame->linesize[0], frame->height, frame->data[0], frame->data[1], frame->data[2]);
            av_frame_free(&frame);
            av_free(frame);
            frame = NULL;
        }
    }
    isExit = true;

}

void CCLVideo::release() {
    if(LOG_IS_OPEN)
    {
        LOGE("开始释放audio ...");
    }

    if (cclPlayStatus != NULL) {
        cclPlayStatus->exit = true;
    }
    if (queue != NULL) {
        queue->noticeThread();
    }
    int count = 0;
    while (!isExit || !isExit2)
    {
        if (LOG_IS_OPEN) {
           LOGE("等待渲染线程结束...%d", count);
        }
        if (count > 1000)
        {
            isExit2 = true;
            isExit = true;
        }
        count ++;
        av_usleep(1000 * 10);
    }

    if (queue != NULL) {
        queue->release();
        delete (queue);
        queue = NULL;
    }
    if (cclJavaCall != NULL) {
        cclJavaCall = NULL;
    }

    if (cclAudio != NULL) {
        cclAudio = NULL;
    }
    if (avCodecContext != NULL) {
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext = NULL;
    }

    if (cclPlayStatus != NULL) {
        cclPlayStatus = NULL;
    }


}

double CCLVideo::synchronize(AVFrame *srcFrame, double pts) {
    double frame_delay;
    if (pts != 0) {
        video_clock = pts;
    } else {
        pts = video_clock;
    }
    frame_delay = av_q2d(time_base);
    frame_delay += srcFrame->repeat_pict * (frame_delay * 0.5);
    video_clock += frame_delay;
    return pts;

}

double CCLVideo::getDelayTime(double diff) {

    if(LOG_IS_OPEN)
    {
        LOGD("audio video diff is %f", diff);
    }
    if (diff > 0.003)
    {
        delayTime = delayTime /3 *2;
        if (delayTime < rate / 2)
        {
            delayTime = rate / 3 *2;
        } else if (delayTime > rate *2) {
            delayTime = rate * 2;
        }
    } else if (diff <-0.003)
    {
        delayTime = delayTime *3 /2;
        if (delayTime < rate /2)
        {
            delayTime = rate /3 *2;
        } else if (delayTime > rate *2)
        {
            delayTime = rate *2;
        }
    } else if(diff == 0) {
        delayTime = rate;
    }
    if (diff > 1.0) {
        delayTime = 0;
    }
    if (diff < -1.0) {
        delayTime = rate *2;
    }
    if (fabs(diff) > 10)
    {
        delayTime = rate;
    }
    return delayTime;
}

void CCLVideo::setClock(int secds) {
        clock = secds;
}
