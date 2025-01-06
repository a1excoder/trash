#ifndef SERIAL_H
#define SERIAL_H

#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

#include "memory.h"

#define SERIALCOMM TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM")

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 1024

DWORD get_serial_ports(DWORD *number_ports, PVOID *pNullBuff) {
	HKEY comm = NULL;

	DWORD max_value_len = 0;
	DWORD max_key_len = 0;

	TCHAR value_buff[MAX_VALUE_NAME];
	DWORD value_buff_size = MAX_VALUE_NAME;

	TCHAR key_buff[MAX_KEY_LENGTH] = { 0 };
	DWORD key_buff_size = MAX_KEY_LENGTH;

	LSTATUS res = ERROR_SUCCESS;
	DWORD ret_code = ERROR_SUCCESS;

	res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, SERIALCOMM, 0, KEY_READ, &comm);

	if (res == ERROR_SUCCESS) {
		ret_code = RegQueryInfoKey(
			comm,
			NULL, NULL, NULL, NULL, NULL, NULL,
			number_ports,
			&max_key_len,
			&max_value_len,
			NULL, NULL);

		if (ret_code == ERROR_SUCCESS) {
			max_key_len += 1;

			*pNullBuff = heap_alloc((*number_ports * ((max_key_len * sizeof(TCHAR)) + max_value_len)) + (sizeof(DWORD) * 2));
			if (*pNullBuff == NULL) return 3;

			memcpy(*pNullBuff, &max_key_len, sizeof(DWORD));
			memcpy((BYTE*)(*pNullBuff) + sizeof(DWORD), &max_value_len, sizeof(DWORD));

			if (*number_ports) {
				for (DWORD i = 0, ret_code = ERROR_SUCCESS, res = ERROR_SUCCESS; i < *number_ports; ++i) {
					key_buff_size = MAX_KEY_LENGTH;
					key_buff[0] = '\0';

					value_buff_size = MAX_VALUE_NAME;
					value_buff[0] = '\0';

					ret_code = RegEnumValue(comm, i,
						key_buff,
						&key_buff_size,
						NULL, NULL, NULL, NULL);

					if (ret_code == ERROR_SUCCESS)
					{
						res = RegGetValue(comm, NULL, key_buff, RRF_RT_REG_SZ, NULL, value_buff, &value_buff_size);
						if (res == ERROR_SUCCESS) {

							memcpy((BYTE*)(*pNullBuff) + sizeof(DWORD) * 2 + i * ((max_key_len * sizeof(TCHAR)) + max_value_len),
								key_buff, key_buff_size * sizeof(TCHAR) + sizeof(TCHAR));
							
							memcpy((BYTE*)(*pNullBuff) + sizeof(DWORD) * 2 + i * ((max_key_len * sizeof(TCHAR)) + max_value_len) + (max_key_len * sizeof(TCHAR)),
								value_buff, value_buff_size);
						}
						
					}
				}
			}

		}
		else return 2;

		if ((res = RegCloseKey(comm)) != ERROR_SUCCESS) return 3;
	}
	else return 1;

	return 0;
}

void list_ports(DWORD num_of_ports, DWORD max_key_len, DWORD max_value_len, PVOID ports) {
	PVOID pkey = NULL;
	PVOID pvalue = NULL;

	for (DWORD i = 0; i < num_of_ports; i++) {
		pkey = (BYTE*)ports + i * ((max_key_len * sizeof(TCHAR)) + max_value_len);
		pvalue = (BYTE*)ports + i * ((max_key_len * sizeof(TCHAR)) + max_value_len) + (max_key_len * sizeof(TCHAR));

		_tprintf(TEXT("%s \t-\t %s\n"), (PTCHAR)pkey, (PTCHAR)pvalue);
	}
}

DWORD find_device_by_signature() {
	return 0;
}

DWORD free_serial_ports(PVOID* data) {
	return heap_free(*data) ? 0 : 1;
}

#endif SERIAL_H