#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <tchar.h>
#include <ShlObj_core.h>

// int32_t get_env(const TCHAR* env_name, TCHAR* str_buff, int32_t buff_size);

int32_t get_downloads_folder(PWSTR* buff);
void free_downloads_folder(PWSTR buff);

// TRUE = set | FALSE = unset
int32_t auto_run(const TCHAR* var_name, const BOOL set_or_del);

int32_t init_dirs(const TCHAR* main_dir, BOOL is_strict);

#endif