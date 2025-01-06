#include <tchar.h>
#include <Windows.h>

#include "serial.h"

int main(int argc, const TCHAR** argv) {

	// HANDLE Port = CreateFile(port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	DWORD num_of_ports = 0;
	PVOID ports = NULL;

	DWORD max_key_len = 0;
	DWORD max_value_len = 0;

	DWORD code = get_serial_ports(&num_of_ports, &ports); // 0 = success

	DCB dcb_st = {.DCBlength = sizeof(DCB)};

	if (!code) {
		max_key_len = (DWORD) * (DWORD*)(ports);
		max_value_len = (DWORD) * ((DWORD*)(ports) + 1);

		list_ports(num_of_ports, max_key_len, max_value_len, (BYTE*)ports + sizeof(DWORD) * 2);
		// code = 
		// if (code) {
		//		printf("list_ports error - %d\n", code);
		// }
	} else printf("get_serial_ports error - %lu\n", code);

	HANDLE _hPort = CreateFile(TEXT("\\\\.\\COM14"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	// SetCommMask(_hPort, EV_RXCHAR);
	// SetupComm(_hPort, 64, 64);
	// PurgeComm(_hPort,PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

	if (_hPort == INVALID_HANDLE_VALUE) {
		printf("COM14 port invalid\n");
	}
	else {
		if (!GetCommState(_hPort, &dcb_st)) {
			printf("GetCommState error\n");
		}
		else {
			printf("BaudRate is %lu\n", dcb_st.BaudRate);
		}
	}

	CloseHandle(_hPort);

	return free_serial_ports(&ports);
}
