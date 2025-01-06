#include "utils.h"

#include <assert.h>
#include <strsafe.h>

#define AUTORUN TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run")

#define DIR_MAIN TEXT("\\Filer")

#define DIR_PICTURES TEXT("\\pictures")
#define DIR_VIDEOS TEXT("\\videos")
#define DIR_AUDIO TEXT("\\audio")
#define DIR_ARCHIVES TEXT("\\archives")
#define DIR_DOCUMENTS TEXT("\\documents")
#define DIR_EXECUTABLE TEXT("\\executable")
#define DIR_OTHER TEXT("\\other")

#define ADDITIONALLY_PATH (64 * sizeof(TCHAR))

static const TCHAR dirs[][32] = {
	DIR_PICTURES, 
	DIR_VIDEOS, 
	DIR_AUDIO, 
	DIR_ARCHIVES, 
	DIR_DOCUMENTS, 
	DIR_EXECUTABLE, 
	DIR_OTHER
};


#ifdef UNICODE
#define DIR_STRSTR wcsstr
#define DIR_STRCHR wcschr

#else
#define DIR_STRSTR strstr
#define DIR_STRCHR strchr

#endif


enum FORMARTS_ENUM {
	executable = 0,
	documents,
	images,
	audio,
	videos,
	archives,
	disk_images,
	other, // number of types
};

#define FORMATS_FILE_END_NUMBER 0x10
#define FORMATS_FILE_END_STRING_SIZE 0x10

static const int formats_number[] = {
	2,
	6,
	3,
	2,
	4,
	6,
	3
};

static const TCHAR formats_end[][FORMATS_FILE_END_NUMBER][FORMATS_FILE_END_STRING_SIZE] = {
	{TEXT("exe"), TEXT("msi")},
	{TEXT("doc"), TEXT("docx"), TEXT("accdb"), TEXT("xlsx"), TEXT("pptx"), TEXT("ppt")},
	{TEXT("png"), TEXT("jpg"), TEXT("gif")},
	{TEXT("mp3"), TEXT("ogg")},
	{TEXT("mp4"), TEXT("avi"), TEXT("mov"), TEXT("webp")},
	{TEXT("7z"), TEXT("rar"), TEXT("zip"), TEXT("tar"), TEXT("gz"), TEXT("bz2")},
	{TEXT("iso"), TEXT("dmg"), TEXT("raw")}
};

typedef struct {
	enum FORMARTS_ENUM type;
	const TCHAR* format;
} RESULT_FORMAT;



static TCHAR* find_last_dot(const TCHAR* str)
{
	assert(str != NULL);

	TCHAR* res = NULL;
	TCHAR* tmp = str;


	while ((tmp = DIR_STRCHR(tmp, L'.')) != NULL) {
		res = tmp = tmp + 1;
	}

	return res;
}

static RESULT_FORMAT find_format(const TCHAR* file_name)
{
	assert(file_name != NULL);

	RESULT_FORMAT res = {.type = other, .format = find_last_dot(file_name)};
	
	if (res.format != NULL) {
		for (int i = 0; i < other; i++) {
			for (int j = 0; j < formats_number[i]; j++) {

				if ((res.format = DIR_STRSTR(file_name, formats_end[i][j])) != NULL) {
					res.type = i;
					
					return res;
				}
			}
		}
	}

	res.format = NULL;
	return res;
}

// max regedit name if value is 32 767 bytes
#define MAX_REG 512 // symbols





/*
 *
 *	NOT THREADING SAFE
 *
*/

LSTATUS lstatus_check = ERROR_SUCCESS;
#define CHECK_RESULT(func_res) \
	lstatus_check = func_res; \
	if (lstatus_check != ERROR_SUCCESS) \
		return (int32_t)lstatus_check;

LSTATUS lstatus_check_close = ERROR_SUCCESS;
#define CHECK_RESULT_CLOSE(func_res, hKeyHead) \
	lstatus_check_close = func_res; \
	if (lstatus_check_close != ERROR_SUCCESS) { \
		CHECK_RESULT(RegCloseKey(hKeyHead)) \
		return (int32_t)lstatus_check_close; \
	} \

/*
 *
 *	NOT THREADING SAFE
 *
*/




#define STRLEN_BYTES(str) \
	((lstrlen(str) + 1) * sizeof(TCHAR))


#define DEBUG_MSG(str) \
	MessageBox(NULL, str, TEXT("Debug message"), MB_OK)

// another method for get Downloads directory of HOMEDRIVE\HOMEPATH\Downloads
// if not found -1 , if another error -2
/*
int32_t get_env(const TCHAR* env_name, TCHAR* str_buff, int32_t buff_size)
{
	buff_size = (int32_t)GetEnvironmentVariable(env_name, str_buff, (DWORD)buff_size);

	if (buff_size == 0) {
		if (GetLastError() == ERROR_ENVVAR_NOT_FOUND) {
			return -1;
		}

		return -2;
	}

	// if (buff_size > ...)

	return buff_size;
}
*/

static int32_t get_work_path(TCHAR *pWorkPath, const DWORD nSize)
{
	return GetModuleFileName(NULL, pWorkPath, nSize) == 0 ? -1 : 0;
}

static void* heap_alloc(size_t bytes) {
	assert(bytes >= 1);
	return HeapAlloc(GetProcessHeap(), 0, bytes);
}

static BOOL heap_free(void* lp_heap) {
	assert(lp_heap != NULL);
	return HeapFree(GetProcessHeap(), 0, lp_heap);
}

static BOOL dir_exists(const TCHAR* pPath)
{
	DWORD dw_attrib = GetFileAttributes(pPath);
	return (dw_attrib != INVALID_FILE_ATTRIBUTES && (dw_attrib & FILE_ATTRIBUTE_DIRECTORY));
}

int32_t file_sorter(const TCHAR* folder_from, const TCHAR* pPath)
{
	assert(folder_from != NULL && pPath != NULL);
	// TCHAR szDir [MAX_PATH] = { 0 };
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;

	HANDLE hFind = INVALID_HANDLE_VALUE;

	size_t length_of_path_from = 0L;
	size_t length_of_path_to = 0L;

	if (dir_exists(folder_from) && dir_exists(pPath))
	{
		if (SUCCEEDED(StringCchLength(folder_from, MAX_PATH, &length_of_path_from)) >= 0 && 
			SUCCEEDED(StringCchLength(pPath, MAX_PATH, &length_of_path_to)) >= 0) {

			if (length_of_path_from > (MAX_PATH - 3) || length_of_path_to > (MAX_PATH - 3)) {
				
				hFind = FindFirstFile(folder_from, &ffd);

				if (INVALID_HANDLE_VALUE == hFind) {


					return -1;
				}

				do {
					if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
						filesize.LowPart = ffd.nFileSizeLow;
						filesize.HighPart = ffd.nFileSizeHigh;
						DEBUG_MSG(ffd.cFileName);
						// printf(TEXT("  %s   %ld bytes\n"), ffd.cFileName, filesize.QuadPart);
					}

				} while (FindNextFile(folder_from, &ffd) != 0);

				FindClose(hFind);
			}
		}
	}

	return 0;
}

// TRUE = set | FALSE = unset
int32_t auto_run(const TCHAR *var_name, const BOOL set_or_del)
{
	assert(var_name != NULL);

	TCHAR szFileName[MAX_PATH];
	if (get_work_path(szFileName, MAX_PATH) != 0) return -1;
	
	HKEY kas = NULL;
	LSTATUS res = RegOpenKeyEx(HKEY_CURRENT_USER, AUTORUN, 0, KEY_SET_VALUE, &kas);


	if (res == ERROR_SUCCESS) {
		if (set_or_del == TRUE) {
			CHECK_RESULT_CLOSE(RegSetValueEx(kas, var_name, 0, REG_SZ, (CONST BYTE*)szFileName, STRLEN_BYTES(szFileName)), kas)
		} else {
			CHECK_RESULT_CLOSE(RegDeleteValue(kas, var_name), kas)
		}


		CHECK_RESULT(RegCloseKey(kas))

	} else return (int32_t)res;


	return 0;
}

int32_t get_downloads_folder(PWSTR* buff)
{
	if (SHGetKnownFolderPath(&FOLDERID_Downloads, KF_FLAG_DEFAULT, NULL, buff) != S_OK) return -1;
	return 0;
}

void free_downloads_folder(PWSTR buff)
{
	assert(buff != NULL);

	CoTaskMemFree(buff);
}

int32_t init_dirs(const TCHAR* main_dir, BOOL is_strict)
{
	assert(main_dir != NULL);

	int n = STRLEN_BYTES(main_dir) - sizeof(TCHAR) + STRLEN_BYTES(DIR_MAIN);

	TCHAR* new_dir = (TCHAR*)heap_alloc(n);
	TCHAR* dir_buffer = (TCHAR*)heap_alloc(n + ADDITIONALLY_PATH);

	lstrcpy(new_dir, main_dir);

	StringCchCat(new_dir, n / sizeof(TCHAR), DIR_MAIN);
	BOOL res = CreateDirectory(new_dir, NULL);
	if (res == 0) {
		if (GetLastError() == ERROR_PATH_NOT_FOUND) {
			heap_free(new_dir);
			return -2;
		}

		if (is_strict == TRUE) {
			if (GetLastError() == ERROR_ALREADY_EXISTS) {
				heap_free(new_dir);
				return -1;
			}

			return -3;
		}

	}


	for (int dir = 0; dir < (int)(sizeof(dirs) / sizeof(*dirs)); dir++) {
		lstrcpy(dir_buffer, new_dir);
		StringCchCat(dir_buffer, n + ADDITIONALLY_PATH, dirs[dir]);
		
		res = CreateDirectory(dir_buffer, NULL);
		if (res == 0) {
			if (GetLastError() == ERROR_PATH_NOT_FOUND) {
				heap_free(new_dir);
				return -2;
			}


			if (is_strict == TRUE) {
				if (GetLastError() == ERROR_ALREADY_EXISTS) {
					heap_free(new_dir);
					return -1;
				}

				return -3;
			}
		}
	}


	heap_free(new_dir);
	heap_free(dir_buffer);

	return 0;
}