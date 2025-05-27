#include "../include/PCAN.hpp"
#include <iostream>
#include <cstring>


PCAN::PCAN(PCANDeviceInfo status , bool is_FD) {

    this->status = status;
    this->is_FD = is_FD;
}


TPCANStatus PCAN::Initialize(TPCANBaudrate baudrate) {
    return CAN_Initialize(this->status.channel_handle, baudrate, 0, 0, 0);
}

TPCANStatus PCAN::Uninitialize() {
    return CAN_Uninitialize(this->status.channel_handle);
}


TPCANStatus PCAN::PcanMessageSend(DWORD id, BYTE* data, BYTE length, bool extended) {
    if (length > 8) {
        return PCAN_ERROR_ILLPARAMVAL;
    }

    TPCANMsg message;
    message.ID = id;
    message.MSGTYPE = extended ? PCAN_MESSAGE_EXTENDED : PCAN_MESSAGE_STANDARD;
    message.LEN = length;
    
    for (int i = 0; i < length; i++) {
        message.DATA[i] = data[i];
    }

    return CAN_Write(this->status.channel_handle, &message);
}

TPCANStatus PCAN::PcanMessageReceive(TPCANMsg* message, TPCANTimestamp* timestamp) {
    return CAN_Read(this->status.channel_handle, message, timestamp);
}

TPCANStatus PCAN::GetChannelStatus() {
    return CAN_GetStatus(this->status.channel_handle);
}

PCAN::~PCAN() {
    Uninitialize();
}