//
// Created by meng on 25-7-7.
// DOC 文件IO模块
//

#ifndef FILEIO_H
#define FILEIO_H

#include <string>

#include "dataFormat.h"

class FileIO {
public:
    /**
     * 将车位状态信息加载到内存中
     */
    static bool loadParkStatus(const std::string &statusFileName, ParkStatus parkStatus[]);

    /**
     * 将车位状态信息保存到文件中
     */
    static bool saveParkStatus(const std::string &statusFileName, ParkStatus parkStatus[]);

    /**
     * 将车辆停入信息加载到内存中
     */
    static bool loadParkLog(const std::string &logFileName, ParkInfo *&parkInfo_head, ParkInfo *&parkInfo_tail);

    /**
     * 将车辆停入信息保存到文件中
     */
    static bool saveParkLog(const std::string &logFileName, ParkInfo *parkInfo_head);
};

#endif //FILEIO_H
