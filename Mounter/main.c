#include <Windows.h>
#include <virtdisk.h>

#include <stdio.h>
#include <string.h>

#pragma comment(lib, "VirtDisk.lib")

typedef enum {
	MODE_MOUNT = 0,
	MODE_DISMOUNT,
	MODE_HELP,
	MODE_NULL
} MODE;

MODE start(int, const WCHAR**, int*);
void print_help();

DWORD mount(const WCHAR*, HANDLE*);
BOOL dismount(const WCHAR* volume);


int wmain(int argc, const WCHAR** argv)
{
	DefineDosDevice(DDD_REMOVE_DEFINITION, L"F:\\", NULL);
	return 1;
	VIRTUAL_STORAGE_TYPE disk_data = { 0 };
	HANDLE hdisk = NULL;
	DWORD status = ERROR_SUCCESS;
	int val_i = 0;

	switch (start(argc, argv, &val_i)) {
	case MODE_MOUNT:
		status = mount(argv[val_i], &hdisk);
		if (status != ERROR_SUCCESS) {
			wprintf(L"[ERROR] - file path not found\n");
			return 1;
		}
		system("pause");
		break;
	case MODE_DISMOUNT:
		status = (DWORD)dismount(argv[val_i]);
		if (!status) {
			wprintf(L"[ERROR] - file path not found\n");
			return 2;
		}
		system("pause");
		break;
	case MODE_HELP:
		print_help();
		return 0;
	case MODE_NULL:
		print_help();
		return 0;
	}

	return 0;
}

MODE start(int argc, const WCHAR** argv, int *value_i) {
	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '-') {
			if (!wcscmp(argv[i], L"-h") || !wcscmp(argv[i], L"--help")) {
				return MODE_HELP;
			}

			if (!wcscmp(argv[i], L"-m") || !wcscmp(argv[i], L"--mount")) {
				*value_i = i + 1;
				return MODE_MOUNT;
			}

			if (!wcscmp(argv[i], L"-d") || !wcscmp(argv[i], L"--dismount")) {
				*value_i = i + 1;
				return MODE_DISMOUNT;
			}
			
		}
	}

	return MODE_NULL;
}

DWORD mount(const WCHAR* path, HANDLE* hdisk) {
	VIRTUAL_STORAGE_TYPE disk_data = { 0 };

	DWORD status = OpenVirtualDisk(
		&disk_data,
		path,
		VIRTUAL_DISK_ACCESS_READ,
		OPEN_VIRTUAL_DISK_FLAG_NONE,
		NULL,
		hdisk);

	if (status != ERROR_SUCCESS) return status;


	status = AttachVirtualDisk(
		*hdisk,
		NULL,
		ATTACH_VIRTUAL_DISK_FLAG_READ_ONLY | ATTACH_VIRTUAL_DISK_FLAG_PERMANENT_LIFETIME,
		NULL,
		NULL,
		NULL);

	return status;
}

BOOL dismount(const WCHAR *volume) {
	return DefineDosDeviceW(DDD_REMOVE_DEFINITION, volume, NULL);
	// return DetachVirtualDisk(*hdisk, DETACH_VIRTUAL_DISK_FLAG_NONE, NULL);
}

void print_help() {
	wprintf(
		L"[info about program]	--help (-h)\n"
		L"[mount iso]		--mount (-m) then path to iso\n"
		L"[dismount]		--dismount (-d) then path volume\n"
		L"[example]		Mounter.exe -m D:\\iso\\file.iso\n");
}
