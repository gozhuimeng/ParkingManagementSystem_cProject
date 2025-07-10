//
// Created by meng on 25-7-8.
// DOC �ļ�IOģ��
//

#include "../include/fileIO.h"

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include "../include/dataFormat.h"
#include "../include/config.h"


/**
 * ���복λ״̬��Ϣ���ڴ���
 */
bool FileIO::loadParkStatus(const std::string &statusFileName, ParkStatus parkStatus[]) {
    // �����ļ���
    std::ifstream ParkStatusFile(statusFileName);
    if (!ParkStatusFile.is_open()) {
        std::cerr << "�޷��򿪳�λ״̬��Ϣ�ļ�: " << statusFileName << std::endl;
        return false; // �жϲ���
    }

    // ��ʼ������
    for (int i = 0; i < PARK_MAX_NUMBER; ++i) {
        parkStatus[i].ParkId = i + 1;
        parkStatus[i].status = 0;
        parkStatus[i].CarNum = "";
        parkStatus[i].time = 0;
    }

    // ��ȡ�ļ�����
    std::string line;
    while (std::getline(ParkStatusFile, line)) {
        std::stringstream ss(line); // �����ַ���������
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
    std::cout << "��λ״̬��Ϣ���سɹ���" << std::endl;
    return true; // �ɹ�����
}

/**
 * ���泵λ״̬��Ϣ���ļ���
 */
bool FileIO::saveParkStatus(const std::string &statusFileName, ParkStatus parkStatus[]) {
    // �����ļ���
    std::ofstream ParkStatusFile(statusFileName);
    if (!ParkStatusFile.is_open()) {
        std::cerr << "�޷��򿪳�λ״̬��Ϣ�ļ�: " << statusFileName << std::endl;
        return false;
    }

    // д���ļ�����
    for (int i = 0; i < PARK_MAX_NUMBER; ++i) {
        ParkStatusFile << parkStatus[i].ParkId << ","
                << parkStatus[i].status << ","
                << parkStatus[i].CarNum << ","
                << parkStatus[i].time << "\n";
    }
    ParkStatusFile.close();
    std::cout << "��λ״̬��Ϣ����ɹ���" << std::endl;
    return true;
}

/**
 * ���복��ͣ����Ϣ���ڴ���
 */
bool FileIO::loadParkLog(const std::string &logFileName, ParkInfo *&parkInfo_head, ParkInfo *&parkInfo_tail) {
    // �����ļ���
    std::ifstream ParkLogFile(logFileName);
    if (!ParkLogFile.is_open()) {
        std::cerr << "�޷��򿪳���ͣ����Ϣ�ļ�: " << logFileName << std::endl;
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
    std::cout << "����ͣ����Ϣ���سɹ���" << std::endl;
    return true; // �ɹ�����
}

/**
 * ���泵��ͣ����Ϣ���ļ���
 */
bool FileIO::saveParkLog(const std::string &logFileName, ParkInfo *parkInfo_head) {
    // �����ļ���
    std::ofstream ParkLogFile(logFileName);
    if (!ParkLogFile.is_open()) {
        std::cerr << "�޷��򿪳���ͣ����Ϣ�ļ�: " << logFileName << std::endl;
        return false;
    }
    // д���ļ�����
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
    std::cout << "����ͣ����Ϣ����ɹ���" << std::endl;
    return true;
}
