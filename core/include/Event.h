//
// Created by meng on 25-7-7.
// DOC 事件处理模块
//

#ifndef EVENT_H
#define EVENT_H

#include "config.h"
#include "dataFormat.h"

class Event {
private:
    // 成员变量
    std::string parkStatusFileName; // 车位状态信息文件名
    std::string parkLogFileName; // 车位状态信息文件名和车辆停入信息文件名
    int systemTime; // 系统时间（单位：秒）


    // 辅助功能函数
    /**
     * 检查车牌号是否已存在
     * @param carNum
     * @return
     */
    Result checkCarNumExist(const std::string &carNum);

    /**
     * 计算费用
     * @param startTime
     * @return
     */
    float Cost(int startTime);

public:
    ParkStatus parkStatus[PARK_MAX_NUMBER]; // 车位状态信息数组
    ParkInfo *parkInfo_head = nullptr; // 车辆停入信息链表头指针
    ParkInfo *parkInfo_tail = nullptr; // 车辆停入信息链表尾指针


    // 构造函数
    Event();

    Event(const std::string &statusFile, const std::string &logFile);

    // 功能函数
    /**
     * 停车
     * @param carNum 车牌号
     * @param parkId 车位号
     */
    Result parkCar(const std::string &carNum, const int parkId);

    /**
     * 车辆离开
     * @param carNum 车牌号
     * @param parkId 车位号
     */
    Result carLeave(const std::string &carNum, const int parkId);

    /**
     * 预约车位
     * @param carNum 车牌号
     * @param parkId 车位号
     */
    Result reservation(std::string &carNum, int parkId);

    Result queryFee(const std::string &carNum);

    Result payFee(const std::string &carNum);

    /**
     * 结算日费用
     * @return
     */
    Result settlementFeesByDays();

    /**
     * 结算月费用
     * @return
     */
    Result settlementFeesByMonths();

    /**
     * 结算年费用
     * @return
     */
    Result settlementFeesByYear();

    /**
     * 更新系统时间（为了方便演示所使用的功能）
     * @param time 系统时间戳（单位：秒）
     */
    void updateTime(int time);
};

#endif //EVENT_H
