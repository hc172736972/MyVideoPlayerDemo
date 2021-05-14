//
// Created by chencl on 21-5-11.
//

#include "CCLAudioChannel.h"

CCLAudioChannel::CCLAudioChannel(int id, AVRational base) {

    channelId = id;
    time_base = base;
}

CCLAudioChannel::CCLAudioChannel(int id, AVRational base, int fp) {
   channelId = id;
   time_base = base;
   fps = fp;
}

CCLAudioChannel::~CCLAudioChannel() {

}
