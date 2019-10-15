#ifndef _FS_UTILS_H_
#define _FS_UTILS_H_

#include "utils/types.h"
#include "libs/fatfs/ff.h"
#include "storage/sdmmc.h"
#include "storage/sdmmc_driver.h"

sdmmc_t g_sd_sdmmc;
sdmmc_storage_t g_sd_storage;
FATFS g_sd_fs;
bool g_sd_mounted;

bool sd_mount();
void sd_unmount();
void *sd_file_read(char *path);
int sd_save_to_file(void *buf, u32 size, const char *filename);
bool sd_file_exists(const char* filename);
void copyfile(const char* source, const char* target);
void copyfileparam(char* param, char* source, char* target);
void copy_folder(char* sourse_folder, char* dest_folder);
bool sd_file_size(char *path);
#endif
