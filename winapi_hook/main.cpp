#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>

#define DEBUG_MODE

#include "own_debug.h"

SIZE_T bwritten = 0;
FARPROC orig_func_addr = nullptr;

#ifdef _WIN64
UINT8 orig_func_data[12] = { 0 };
#else
UINT8 orig_func_data[6] = { 0 };
#endif

// typedef int (WINAPI *fnMessageBoxA)(HWND, LPCSTR, LPCSTR, UINT);

int WINAPI HookedMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
	PDEBUG("message from hooked MessageBoxA function", "INFO");

	// unhook
	WriteProcessMemory(GetCurrentProcess(), static_cast<PVOID>(orig_func_addr), static_cast<LPCVOID>(orig_func_data), sizeof(orig_func_data), &bwritten);
	if (bwritten != sizeof(orig_func_data)) {
		ExitProcess(5);
	}

	return MessageBoxA(hWnd, "HOOKED", "HOOKED", uType);
}


int main()
{	
	HMODULE WINAPI user32 = LoadLibraryA("user32.dll");
	if (nullptr == user32) {
		PDEBUG("user32.dll cannot be loaded by LoadLibraryA", "WARNING");
		ExitProcess(1);
	}

	orig_func_addr = GetProcAddress(user32, "MessageBoxA");
	if (nullptr == orig_func_addr) {
		PDEBUG("MessageBoxA cannot be found by GetProcAddress", "WARNING");
		ExitProcess(2);
	}

	SIZE_T bread = 0;
	ReadProcessMemory(GetCurrentProcess(), static_cast<LPCVOID>(orig_func_addr), static_cast<PVOID>(orig_func_data), sizeof(orig_func_data), &bread);
	if (bread != sizeof(orig_func_data)) {
		ExitProcess(3);
	}

	PVOID ptr_hooked_func = &HookedMessageBoxA;

#ifdef _WIN64
	UINT8 path_buff[12] = { 0 };
	memcpy_s(path_buff, sizeof(path_buff), static_cast<LPCVOID>("\x48\xB8"), 2); // movabs rax, 0x...
	memcpy_s(path_buff + 2, sizeof(path_buff) - 2, static_cast<LPCVOID>(&ptr_hooked_func), sizeof(ptrdiff_t));

	memcpy_s(path_buff + 10, sizeof(path_buff) - 10, static_cast<LPCVOID>("\x50"), 1); // push rax
	memcpy_s(path_buff + 11, sizeof(path_buff) - 11, static_cast<LPCVOID>("\xC3"), 1); // ret
#else
	UINT8 path_buff[6] = { 0 };
	memcpy_s(path_buff, sizeof(path_buff), static_cast<LPCVOID>("\x68"), 1); // push
	memcpy_s(path_buff + 1, sizeof(path_buff) - 1, static_cast<LPCVOID>(&ptr_hooked_func), sizeof(ptrdiff_t));
	memcpy_s(path_buff + 5, sizeof(path_buff) - 5, static_cast<LPCVOID>("\xC3"), 1); // ret
#endif
	
	WriteProcessMemory(GetCurrentProcess(), static_cast<PVOID>(orig_func_addr), static_cast<LPCVOID>(path_buff), sizeof(path_buff), &bwritten);

	if (bwritten != sizeof(path_buff)) {
		ExitProcess(4);
	}

	// reinterpret_cast<fnMessageBoxA>(ptr_hooked_func)(nullptr, "data", "title", MB_OK);
	MessageBoxA(nullptr, "fuck you", "Hello", MB_OK);

	return 0;
}
