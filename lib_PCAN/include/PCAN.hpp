#pragma once
#include <windows.h>
#include "PCANBasic.h"
#include <iostream>
#include <vector>
#include <string>

struct PCANDeviceInfo {
    TPCANHandle channel_handle;        // PCAN 通道句柄
    TPCANDevice device_type;           // 设备类型 (如 PCAN_USB, PCAN_PCI 等)
    BYTE controller_number;            // CAN 控制器编号
    DWORD device_features;             // 设备特性标志
    std::string device_name;           // 设备名称
    DWORD device_id;                   // 设备 ID 号
    DWORD channel_condition;           // 通道状态
};





class PCAN {
public:
    PCAN(PCANDeviceInfo status , bool is_FD = false);

    PCAN() = delete;

    TPCANStatus Initialize(TPCANBaudrate baudrate);

    TPCANStatus Uninitialize();

    TPCANStatus PcanMessageSend(DWORD id, BYTE* data, BYTE length, bool extended = false);

    TPCANStatus PcanMessageReceive(TPCANMsg* message, TPCANTimestamp* timestamp);

    TPCANStatus GetChannelStatus();
    ~PCAN();

    bool is_FD = false;
    PCANDeviceInfo  status;
};



inline void PrintPCANDeviceInfo(const PCANDeviceInfo& dev) {
    std::cout << "PCAN Device Info:" << std::endl;
    std::cout << "  Channel Handle: 0x" << std::hex << dev.channel_handle << std::dec << std::endl;
    std::cout << "  Device Type: " << dev.device_type << std::endl;
    std::cout << "  Controller Number: " << static_cast<int>(dev.controller_number) << std::endl;
    std::cout << "  Device Features: 0x" << std::hex << dev.device_features << std::dec << std::endl;
    std::cout << "  Device Name: " << dev.device_name << std::endl;
    std::cout << "  Device ID: " << dev.device_id << std::endl;
    std::cout << "  Channel Condition: " << dev.channel_condition << std::endl;
}


    // 检测并返回所有连接的 PCAN 设备
    std::vector<PCANDeviceInfo> GetAttachedDevices();
    
    // 获取错误信息文本
inline std::string GetErrorText(TPCANStatus error) {
    char buffer[256];
    if (CAN_GetErrorText(error, 0x09, buffer) != PCAN_ERROR_OK) {
        return "无法获取错误描述";
    }
    return std::string(buffer);
}


inline std::vector<PCANDeviceInfo> GetAttachedDevices() {
    std::vector<PCANDeviceInfo> devices;
    DWORD deviceCount = 0;
    TPCANStatus status;

    // 1. 获取连接的设备数量
    status = CAN_GetValue(PCAN_NONEBUS, PCAN_ATTACHED_CHANNELS_COUNT, &deviceCount, sizeof(deviceCount));
    if (status != PCAN_ERROR_OK) {
        std::cerr << "获取设备数量失败: " << GetErrorText(status) << std::endl;
        return devices;
    }

    // 如果没有设备连接
    if (deviceCount == 0) {
        return devices;
    }

    // 2. 分配足够的内存来存储设备信息
    TPCANChannelInformation* channelInfos = new TPCANChannelInformation[deviceCount];
    memset(channelInfos, 0, deviceCount * sizeof(TPCANChannelInformation));

    // 3. 获取所有设备的信息
    status = CAN_GetValue(PCAN_NONEBUS, PCAN_ATTACHED_CHANNELS, 
                         channelInfos, deviceCount * sizeof(TPCANChannelInformation));
    
    if (status != PCAN_ERROR_OK) {
        std::cerr << "获取设备信息失败: " << GetErrorText(status) << std::endl;
        delete[] channelInfos;
        return devices;
    }

    // 4. 转换为我们自定义的结构体
    for (DWORD i = 0; i < deviceCount; i++) {
        PCANDeviceInfo device;
        device.channel_handle = channelInfos[i].channel_handle;
        device.device_type = channelInfos[i].device_type;
        device.controller_number = channelInfos[i].controller_number;
        device.device_features = channelInfos[i].device_features;
        device.device_name = channelInfos[i].device_name;
        device.device_id = channelInfos[i].device_id;
        device.channel_condition = channelInfos[i].channel_condition;
        
        devices.push_back(device);
    }

    delete[] channelInfos;
    return devices;
}