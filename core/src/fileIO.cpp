//
// Created by meng on 25-7-8.
// DOC 文件IO模块
//

#include "../include/fileIO.h"

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include "../include/dataFormat.h"
#include "../include/config.h"


/**
 * 载入车位状态信息表到内存中
 */
bool FileIO::loadParkStatus(const std::string &statusFileName, ParkStatus parkStatus[]) {
    // 创建文件流
    std::ifstream ParkStatusFile(statusFileName);
    if (!ParkStatusFile.is_open()) {
        std::cerr << "无法打开车位状态信息文件: " << statusFileName << std::endl;
        return false; // 中断操作
    }

    // 初始化数组
    for (int i = 0; i < PARK_MAX_NUMBER; ++i) {
        parkStatus[i].ParkId = i + 1;
        parkStatus[i].status = 0;
        parkStatus[i].CarNum = "";
        parkStatus[i].time = 0;
    }

    // 读取文件内容
    std::string line;
    while (std::getline(ParkStatusFile, line)) {
        std::stringstream ss(line); // 创建字符串流对象
        std::string ParkId_str, status_str, CarNum, time_str;
        std::getline(ss, ParkId_str, ',');
        std::getline(ss, status_str, ',');
        std::getline(ss, CarNum, ',');
        std::getline(ss, time_str, ',');

        int ParkId = std::stoi(ParkId_str);
        int status = std::stoi(status_str);
        int time = std::stoi(time_str);

        parkStatus[ParkId - 1].status = status;
        parkStatus[ParkId - 1].CarNum = CarNum;
        parkStatus[ParkId - 1].time = time;
    }
    ParkStatusFile.close();
    std::cout << "车位状态信息加载成功。" << std::endl;
    return true; // 成功加载
}

/**
 * 保存车位状态信息表到文件中
 */
bool FileIO::saveParkStatus(const std::string &statusFileName, ParkStatus parkStatus[]) {
    // 创建文件流
    std::ofstream ParkStatusFile(statusFileName);
    if (!ParkStatusFile.is_open()) {
        std::cerr << "无法打开车位状态信息文件: " << statusFileName << std::endl;
        return false;
    }

    // 写入文件内容
    for (int i = 0; i < PARK_MAX_NUMBER; ++i) {
        ParkStatusFile << parkStatus[i].ParkId << ","
                << parkStatus[i].status << ","
                << parkStatus[i].CarNum << ","
                << parkStatus[i].time << "\n";
    }
    ParkStatusFile.close();
    std::cout << "车位状态信息保存成功。" << std::endl;
    return true;
}

/**
 * 载入车辆停入信息表到内存中
 */
bool FileIO::loadParkLog(const std::string &logFileName, ParkInfo *&parkInfo_head, ParkInfo *&parkInfo_tail) {
    // 创建文件流
    std::ifstream ParkLogFile(logFileName);
    if (!ParkLogFile.is_open()) {
        std::cerr << "无法打开车辆停入信息文件: " << logFileName << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(ParkLogFile, line)) {
        std::stringstream ss(line);
        std::string CarNum, ParkId_str, ParkingTime_str, OutTime_str, Cost_str, isPaid_str;

        std::getline(ss, CarNum, ',');
        std::getline(ss, ParkId_str, ',');
        std::getline(ss, ParkingTime_str, ',');
        std::getline(ss, OutTime_str, ',');
        std::getline(ss, Cost_str, ',');
        std::getline(ss, isPaid_str, ',');

        int ParkId = std::stoi(ParkId_str);
        int ParkingTime = std::stoi(ParkingTime_str);
        int OutTime = std::stoi(OutTime_str);
        float Cost = std::stof(Cost_str);
        bool isPaid = (isPaid_str == "1");

        ParkInfo *ParkInfo_temp = new ParkInfo{CarNum, ParkId, ParkingTime, OutTime, Cost, isPaid};
        ParkInfo_temp->next = nullptr;
        if (parkInfo_head == nullptr) {
            parkInfo_head = parkInfo_tail = ParkInfo_temp;
        } else {
            parkInfo_tail->next = ParkInfo_temp;
            parkInfo_tail = ParkInfo_temp;
        }
    }
    ParkLogFile.close();
    std::cout << "车辆停入信息加载成功。" << std::endl;
    return true; // 成功加载
}

/**
 * 保存车辆停入信息表到文件中
 */
bool FileIO::saveParkLog(const std::string &logFileName, ParkInfo *parkInfo_head) {
    // 创建文件流
    std::ofstream ParkLogFile(logFileName);
    if (!ParkLogFile.is_open()) {
        std::cerr << "无法打开车辆停入信息文件: " << logFileName << std::endl;
        return false;
    }
    // 写入文件内容
    ParkInfo *current = parkInfo_head;
    while (current != nullptr) {
        ParkLogFile << current->CarNum << ","
                << current->ParkId << ","
                << current->ParkingTime << ","
                << current->OutTime << ","
                << current->Cost << ","
                << (current->isPaid ? "1" : "0") << "\n";
        current = current->next;
    }
    ParkLogFile.close();
    std::cout << "车辆停入信息保存成功。" << std::endl;
    return true;
}
