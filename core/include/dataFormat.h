//
// Created by meng on 25-7-7.
// DOC 基本数据格式定义
//

#ifndef DATAFORMAT_H
#define DATAFORMAT_H

#include <string>

// 车位状态信息节点
typedef struct {
    int ParkId; // 车位ID
    int status; // 车位状态：0-空闲，1-占用，2-已被预约
    std::string CarNum; // 车牌号
    int time; // 车辆停入事件/预约时间
} ParkStatus;

// 车辆停入信息表
typedef struct node {
    std::string CarNum; // 车牌号
    int ParkId; // 车位ID
    int ParkingTime; // 停车时间
    int OutTime; // 离开时间
    float Cost; // 停车费用
    bool isPaid; // 是否已支付
    node *next; // 指向下一个事件节点
} ParkInfo;

#endif //DATAFORMAT_H
