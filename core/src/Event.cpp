//
// Created by meng on 25-7-8.
// DOC 事件处理模块
//

#include "../include/Event.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "../include/dataFormat.h"
#include "../include/fileIO.h"
#include "../include/config.h"


// 构造函数
Event::Event(): Event(PARK_STATUS_FILE_NAME, PARK_LOG_FILE_NAME) {
}

Event::Event(const std::string &statusFile, const std::string &logFile) : parkStatusFileName(statusFile),
                                                                          parkLogFileName(logFile),
                                                                          systemTime(std::time(nullptr)) {
    FileIO::loadParkStatus(parkStatusFileName, parkStatus);
    FileIO::loadParkLog(parkLogFileName, parkInfo_head, parkInfo_tail);
}

/**
 * 停车
 * @param carNum 车牌号
 * @param parkId 车位号
 * @return
 */
Result Event::parkCar(const std::string &carNum, const int parkId) {
    Result result{0, ""};
    int code = checkCarNumExist(carNum).code;
    int id = parkId - 1;
    if (code == -2) {
        result.code = 1;
        result.message = "车牌号已存在，请检查输入！";
        return result;
    }
    if (code != -1) {
        result.code = 1;
        result.message = "该车牌号已在" + std::to_string(code + 1) + "号车位预约，请停至对应车位，不要占用其他车位！";
        parkStatus[id].status = 1;
        parkStatus[id].time = systemTime;
        parkStatus[id].CarNum = carNum;
        FileIO::saveParkStatus(parkStatusFileName, parkStatus);
        return result;
    }
    if (parkStatus[id].status == 0 || (parkStatus[id].status == 2 && parkStatus[id].CarNum == carNum)) {
        parkStatus[id].status = 1;
        parkStatus[id].time = systemTime;
        parkStatus[id].CarNum = carNum;
        result.message = "车牌号：" + carNum + " 停车成功！";
        FileIO::saveParkStatus(parkStatusFileName, parkStatus);
        return result;
    }
    result.code = 1;
    result.message = "车位已被占用或预约，请选择其他车位！";
    return result;
}

/**
 * 驶离
 * @param carNum
 * @param parkId
 * @return
 */
Result Event::carLeave(const std::string &carNum, const int parkId) {
    Result result{0, ""};
    int id = parkId - 1;

    if (parkStatus[id].status != 1 || parkStatus[id].CarNum != carNum) {
        result.code = 1;
        result.message = "信息不匹配，请检查输入！";
        return result;
    }

    float cost = Cost(parkStatus[id].time);

    ParkInfo *temp = new ParkInfo{carNum, parkId, parkStatus[id].time, systemTime, cost, false};
    temp->next = nullptr;

    if (parkInfo_head == nullptr) {
        parkInfo_head = temp;
        parkInfo_tail = temp;
    } else {
        parkInfo_tail->next = temp;
        parkInfo_tail = temp;
    }

    parkStatus[id].status = 0;
    parkStatus[id].time = 0;
    parkStatus[id].CarNum = "";

    result.message = "车牌号：" + carNum + " 一路顺风！";
    FileIO::saveParkStatus(parkStatusFileName, parkStatus);
    FileIO::saveParkLog(parkLogFileName, parkInfo_head);
    return result;
}

/**
 * 预约车位
 * @param carNum
 * @param parkId
 * @return
 */
Result Event::reservation(std::string &carNum, int parkId) {
    Result result{0, ""};
    int id = parkId - 1;
    Result checkResult = checkCarNumExist(carNum);
    if (checkResult.code == -2) {
        result.code = 1;
        result.message = checkResult.message;
        return result;
    }
    if (checkResult.code != -1) {
        result.code = 1;
        result.message = "该车牌号已在" + std::to_string(checkResult.code + 1) + "号车位预约，请勿重复预约";
        return result;
    }
    parkStatus[id].status = 2;
    parkStatus[id].time = systemTime;
    parkStatus[id].CarNum = carNum;
    result.message = "车牌号：" + carNum + " 预约车位：" + std::to_string(parkId) + "成功！";
    FileIO::saveParkStatus(parkStatusFileName, parkStatus);
    return result;
}

/**
 * 查询费用
 * @param carNum
 * @return
 */
Result Event::queryFee(const std::string &carNum) {
    Result result{0, ""};
    bool found = false;
    float total = 0;

    for (ParkInfo *p = parkInfo_head; p != nullptr; p = p->next) {
        if (p->CarNum == carNum && !p->isPaid) {
            total += p->Cost;
            found = true;
        }
    }

    if (!found) {
        result.code = 1;
        result.message = "未查询到该车牌的未支付记录。";
    } else {
        std::ostringstream oss;
        oss << "车牌号 " << carNum << " 未支付总费用：¥" << std::fixed << std::setprecision(2) << total;
        result.message = oss.str();
    }

    return result;
}

/**
 * 缴费
 * @param carNum
 * @return
 */
Result Event::payFee(const std::string &carNum) {
    Result result{0, ""};
    bool found = false;

    for (ParkInfo *p = parkInfo_head; p != nullptr; p = p->next) {
        if (p->CarNum == carNum && !p->isPaid) {
            p->isPaid = true;
            found = true;
        }
    }

    if (!found) {
        result.code = 1;
        result.message = "没有找到未支付的记录。";
    } else {
        result.message = "车牌号 " + carNum + " 缴费成功！";
        FileIO::saveParkLog(parkLogFileName, parkInfo_head);
    }

    return result;
}


/**
 * 结算日收益
 * @return
 */
Result Event::settlementFeesByDays() {
    float total = 0, paid = 0, unpaid = 0;

    std::time_t now_t = static_cast<time_t>(systemTime);
    std::tm now_tm = *std::localtime(&now_t);
    now_tm.tm_hour = 0;
    now_tm.tm_min = 0;
    now_tm.tm_sec = 0;
    std::time_t day_start = std::mktime(&now_tm);
    std::time_t day_end = day_start + 24 * 3600;

    for (ParkInfo *p = parkInfo_head; p != nullptr; p = p->next) {
        std::time_t t = static_cast<time_t>(p->OutTime);
        if (t >= day_start && t < day_end) {
            total += p->Cost;
            if (p->isPaid)
                paid += p->Cost;
            else
                unpaid += p->Cost;
        }
    }

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "【日结算】\n总收入: " << total << " 元\n已支付: " << paid << " 元\n未支付: " << unpaid << " 元";
    return {0, ss.str()};
}

/**
 * 结算月收益
 * @return
 */
Result Event::settlementFeesByMonths() {
    float total = 0, paid = 0, unpaid = 0;

    std::time_t now_t = static_cast<time_t>(systemTime);
    std::tm now_tm = *std::localtime(&now_t);
    now_tm.tm_mday = 1;
    now_tm.tm_hour = 0;
    now_tm.tm_min = 0;
    now_tm.tm_sec = 0;
    std::time_t month_start = std::mktime(&now_tm);

    // 计算下个月开始时间
    if (now_tm.tm_mon == 11) {
        now_tm.tm_year += 1;
        now_tm.tm_mon = 0;
    } else {
        now_tm.tm_mon += 1;
    }
    std::time_t month_end = std::mktime(&now_tm);

    for (ParkInfo *p = parkInfo_head; p != nullptr; p = p->next) {
        std::time_t t = static_cast<time_t>(p->OutTime);
        if (t >= month_start && t < month_end) {
            total += p->Cost;
            if (p->isPaid)
                paid += p->Cost;
            else
                unpaid += p->Cost;
        }
    }

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "【月结算】\n总收入: " << total << " 元\n已支付: " << paid << " 元\n未支付: " << unpaid << " 元";
    return {0, ss.str()};
}

/**
 * 结算年收益
 * @return
 */
Result Event::settlementFeesByYear() {
    float total = 0, paid = 0, unpaid = 0;

    std::time_t now_t = static_cast<time_t>(systemTime);
    std::tm now_tm = *std::localtime(&now_t);
    now_tm.tm_mon = 0;
    now_tm.tm_mday = 1;
    now_tm.tm_hour = 0;
    now_tm.tm_min = 0;
    now_tm.tm_sec = 0;
    std::time_t year_start = std::mktime(&now_tm);

    now_tm.tm_year += 1;
    std::time_t year_end = std::mktime(&now_tm);

    for (ParkInfo *p = parkInfo_head; p != nullptr; p = p->next) {
        std::time_t t = static_cast<time_t>(p->OutTime);
        if (t >= year_start && t < year_end) {
            total += p->Cost;
            if (p->isPaid)
                paid += p->Cost;
            else
                unpaid += p->Cost;
        }
    }

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "【年结算】\n总收入: " << total << " 元\n已支付: " << paid << " 元\n未支付: " << unpaid << " 元";
    return {0, ss.str()};
}


/**
 * 修改系统时间（为了方便演示所使用的功能）
 * @param time 新系统时间戳（单位：秒）
 */
void Event::updateTime(int time) {
    systemTime = time;
}

// 辅助功能函数
/**
 * 检查车牌号是否已存在
 * @param carNum
 * @return
 */
Result Event::checkCarNumExist(const std::string &carNum) {
    Result result{0, ""};
    for (int i = 0; i < PARK_MAX_NUMBER; i++) {
        if (parkStatus[i].CarNum == carNum) {
            if (parkStatus[i].status == 1) {
                result.code = -2;
                result.message = "车牌号已存在，请检查输入！";
            } else if (parkStatus[i].status == 2) {
                result.code = i;
                result.message = "车辆已在" + std::to_string(parkStatus[i].ParkId) + "号车位预约，请勿占用其他车位";
            }
            return result; // 车牌号已存在
        }
    }
    result.code = -1;
    result.message = "车牌号不存在，可以停车";
    return result;
}

/**
 * 结算单次费用
 * @param startTime
 * @return
 */
float Event::Cost(const int startTime) {
    int duration = systemTime - startTime;
    int hours = duration / 3600;

    if (duration <= 7200) {
        // 两小时内免费
        return 0.0f;
    }

    float cost = 5.0f + (hours - 2) * 3.0f;
    if (cost > 30.0f) {
        cost = 30.0f;
    }
    return cost;
}
