// quick & dirty PHYSFS wrapper by goebish
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <sstream>

#define PHYSFS_file FILE
#define PHYSFS_uint8 unsigned char
#define PHYSFS_close fclose

int PHYSFS_init();
void PHYSFS_deinit();
bool PHYSFS_isDirectory(const char* fname);
bool PHYSFS_exists(const char* fname);
bool PHYSFS_mkdir(const char* path);
bool PHYSFS_delete(const char* fname);
std::string PHYSFS_getWriteDir();
std::string PHYSFS_getDirSeparator();
void PHYSFS_addToSearchPath(const char* path,int unknown);
std::string PHYSFS_getUserDir();
void PHYSFS_setWriteDir(const char* path);
void PHYSFS_removeFromSearchPath(const char* path);
PHYSFS_file* PHYSFS_openRead(const char* fname);
PHYSFS_file* PHYSFS_openWrite(const char* fname);
int PHYSFS_fileLength(const char* name);
int PHYSFS_read(PHYSFS_file* filedesc, PHYSFS_uint8* buffer, int count, int length);
void PHYSFS_write(PHYSFS_file* filedesc, PHYSFS_uint8* buffer, int count, int length);



char** PHYSFS_enumerateFiles(const char* path);
void PHYSFS_freeList(char** fileList);

