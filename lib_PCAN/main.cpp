#include <windows.h>
#include "PCANBasic.h"
#include <iostream>
#include "PCAN.hpp"
#include "motor_dm.h"
#include <memory>
void motor_recv_task(PCAN pcan ,std::vector<std::shared_ptr<Motor>>& motors){
	HANDLE iBuffer = CreateEventW(NULL, FALSE, FALSE, L"");
	TPCANStatus stsResult = CAN_SetValue(pcan.status.channel_handle, PCAN_RECEIVE_EVENT, &iBuffer, sizeof(iBuffer));

    if (stsResult != PCAN_ERROR_OK)
    {
        ShowStatus(stsResult);
        return;
    } 
	while (1){
		if (WaitForSingleObject(iBuffer, 50) == WAIT_OBJECT_0){
			CANMessageData message;
	        auto ret = pcan.ReadMessages(&message);
			if (ret == PCAN_ERROR_OK){
				for (auto motor : motors ) {
					// if (auto dm4310 = dynamic_cast<DM4310*>(motor.get())) {
					// 	std::cout << "转换成功，当前电机为 DM4310" << std::endl;
					// 	if (dm4310->Handle_CAN_Message(message)) {
					// 		std::cout << "接收到CAN消息" << std::endl;
					// 	}
					// }
					motor->Handle_CAN_Message(message);
				}
			}
	    }
	}
}
int main(int argc, char* argv[])
{
	auto devices = GetAttachedDevices();
	std::cout << "number of devices: " << devices.size() << std::endl;
	PCANDeviceInfo target_pcan;
	for (auto device : devices){
		PrintPCANDeviceInfo(device);
		target_pcan = device;
	}
	PCAN pcan(target_pcan);
	pcan.Initialize(PCAN_BAUD_1M);
	DM4310 motor(&pcan,MIT_TORQUE_MODE, 0x01, 0x11, SPEED_LOOP);
	// motor.Bind_CAN(&pcan);
	motor.pid_speed.Set_Parameters(0.8,0.1,0.003,0.001);
    motor.pid_speed.Set_Max_Output(10);
    // motor.pid_angle.Set_Parameters(61,0,0,0.001);
    // motor.pid_angle.Enable_ZeroCrossingProtection();
	motor.Enable();
	while(1){
		motor.Set_Speed(5);
		PreciseSleep(1);
	}
	
}