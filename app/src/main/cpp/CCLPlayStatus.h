//
// Created by chencl on 21-5-11.
//

#ifndef CCLPLAYER_CCLPLAYSTATUS_H
#define CCLPLAYER_CCLPLAYSTATUS_H


class CCLPlayStatus {
public:
    bool exit;
    bool pause;
    bool load;
    bool seek;

public:
    CCLPlayStatus();
    ~CCLPlayStatus();
};


#endif //CCLPLAYER_CCLPLAYSTATUS_H
