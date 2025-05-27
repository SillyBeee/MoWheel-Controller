#include <windows.h>
#include "PCANBasic.h"
#include <iostream>
#include "PCAN.hpp"


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
	BYTE data[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC};
	while(true){
		pcan.PcanMessageSend(0x01, data, 8 , false);
		Sleep(1);
	}
}