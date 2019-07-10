#pragma once
#include "FS.h"

#include "consts.h"

class FileStorage
{
public:
	FileStorage(const char* name);
	bool store(const char* str);	
	bool restore(char* str);
private:
	char filename[FILENAME_MAX_LENGTH];
	#ifdef DEBUG_FILE_STORAGE	
	Print *debug = &USE_DEBUG_SERIAL;
	#endif
};