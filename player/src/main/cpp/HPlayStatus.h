//
// Created by 霍振鹏 on 2019-12-24.
// 记录播放状态,方便退出
//

#ifndef RESTUDYAV_HPLAYSTATUS_H
#define RESTUDYAV_HPLAYSTATUS_H


class HPlayStatus {
public:
    bool exit= false;

    bool seek= false;

public:

    HPlayStatus();
    ~HPlayStatus();

};


#endif //RESTUDYAV_HPLAYSTATUS_H
