#include <windows.h>
#include "PCANBasic.h"
#include <iostream>
int main(int argc, char* argv[])
{
	
	TPCANStatus stsResult;
	TPCANMsg msg;
	TPCANTimestamp ts;
	int i = 0;

	// Initialization of the CAN controller
	stsResult = CAN_Initialize(PCAN_USBBUS1, PCAN_BAUD_500K, 0, 0, 0);
	std::cout << "CAN_Initialize: " << stsResult << std::endl;
	
}