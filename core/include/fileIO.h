//
// Created by meng on 25-7-7.
//

#ifndef FILEIO_H
#define FILEIO_H

#include <string>

#include "dataFormat.h"
#include "config.h"

class FileIO {
private:
    std::string parkStatusFileName;  // 车位状态信息文件名
    std::string parkLogFileName;  // 车位状态信息文件名和车辆停入信息文件名
    ParkStatus parkStatus[PARK_MAX_NUMBER];  // 车位状态信息数组
    ParkInfo* parkInfo_head = nullptr; // 车辆停入信息链表头指针
    int parkPtr;  // 车位指针，用于分配车位

public:
    // 构造函数
    FileIO();
    FileIO(const std::string &statusFile, const std::string &logFile);

    // 功能函数
    void openFile();
    void saveFile();

    void loadParkStatus();
    void saveParkStatus();

    void loadParkLog();
    void saveParkLog();
};


#endif //FILEIO_H