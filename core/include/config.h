//
// Created by meng on 25-7-7.
// DOC 基本配置信息
//

#ifndef CONFIG_H
#define CONFIG_H

#include <string>

#define PARK_STATUS_FILE_NAME "./data/parkStatus.csv" // 车位状态信息文件
#define PARK_LOG_FILE_NAME "./data/log.csv"  // 车辆停入信息文件

#define PARK_MAX_NUMBER 100 // 车位最大数量

// 信息返回结构体
typedef struct {
    int code;  // 0-成功，1-失败
    std::string message;  // 返回消息
}Result;

#endif //CONFIG_H
