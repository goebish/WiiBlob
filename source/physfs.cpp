// quick & dirty PHYSFS wrapper by goebish
#include "physfs.h"
#include "fat.h"
#include <sys/stat.h>
#include <string.h>
#include <vector>
#include <dirent.h>
#include <sys/dir.h>
#include <sys/iosupport.h>
#include <ogc/lwp_watchdog.h>
#include <ogc/mutex.h>
#include <ogc/system.h>
#include <sdcard/wiisd_io.h>

#define CACHE 32
#define SECTORS 128
#define BSIZE 1024

std::string writeDir;
std::vector<std::string> searchPath;

int PHYSFS_init()
{
	return fatInitDefault();
}

void PHYSFS_deinit()
{
	
}

bool PHYSFS_isDirectory(const char* fname)
{
	struct stat buf ;
	if (stat (fname,&buf) < 0)
		return false ;
	return (S_ISDIR (buf.st_mode) ? true : false) ;
}

bool PHYSFS_exists(const char* fname) // return true if the file exists and is a regular file
{
	struct stat buf ;
	if (stat (fname,&buf) < 0)
		return false ;
	return (S_ISREG (buf.st_mode) ? true : false) ;
}

bool PHYSFS_mkdir(const char* path)
{
	return mkdir( path, 0700)==0; // return true if ok ? to test ...
}

bool PHYSFS_delete(const char* fname)
{
	return remove(fname)==0; // return true if ok ? to test ...
}

std::string PHYSFS_getWriteDir()
{
	return writeDir.c_str();
}

std::string PHYSFS_getDirSeparator()
{
	std::string sep="/";
	return sep;
}

void PHYSFS_addToSearchPath(const char* path,int unknown)
{
	searchPath.push_back(path);
}

std::string PHYSFS_getUserDir()
{
	std::string userDir = "SD:/data/";
	return userDir;
}

void PHYSFS_setWriteDir(const char* path)
{
	writeDir = path;
}

void PHYSFS_removeFromSearchPath(const char* path)
{
	// TODO
}

PHYSFS_file* PHYSFS_openRead(const char* fname)
{
	// todo : search for fname in search paths
	return fopen( fname, "rb");
}

PHYSFS_file* PHYSFS_openWrite(const char* fname)
{
	// todo : prefix writeDir to fname
	return fopen( fname, "w");
}

int PHYSFS_fileLength(const char* name)
{
	struct stat stbuf;
	if(stat(name, &stbuf) == -1)
		return -1; // The file could not be accessed.
	return stbuf.st_size;
}

int PHYSFS_read(PHYSFS_file* filedesc, PHYSFS_uint8* buffer, int count, int length)
{
	return fread( buffer, length, count, filedesc);
}

void PHYSFS_write(PHYSFS_file* filedesc, PHYSFS_uint8* buffer, int count, int length)
{
	fwrite( buffer, length, count, filedesc);
}

char** PHYSFS_enumerateFiles(const char* path)
{
	return NULL;
}

void PHYSFS_freeList(char** fileList)
{
	
}

