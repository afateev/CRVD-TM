#ifndef FILES_H
#define FILES_H

#include "Drivers.h"

Drivers::FlashType Flash;

int media_read(unsigned long sector, unsigned char *buffer, unsigned long sectorsCount)
{
	if (sectorsCount > 1)
		return 0;
	unsigned long long addr = sector * FAT_SECTOR_SIZE;
	if (!Flash.ReadBlock(addr, buffer, FAT_SECTOR_SIZE))
		return 0;
	return 1;
}

int media_write(unsigned long sector, unsigned char *buffer, unsigned long sectorsCount)
{
	if (sectorsCount > 1)
		return 0;
	unsigned long long addr = sector * FAT_SECTOR_SIZE;
	if (!Flash.WriteBlock(addr, buffer, FAT_SECTOR_SIZE))
		return 0;
	return 1;
}

enum FatState
{
	FatStateInit = 0,
	FatStateError,
    FatStateWaitGuard,
	FatStateReady
};

FatState fatState = FatStateInit;

FL_FILE *eventsFile = 0;
unsigned long eventsFileSize = 0;
FL_FILE *controllerFile = 0;
FL_FILE *oscFile = 0;

struct fatfs		fs_formatting;

bool EventsRead(long int offset, int origin, unsigned char *data, unsigned int count)
{
	if (fatState != FatStateReady)
		return false;
	
	eventsFile = (FL_FILE*)fl_fopen("/events.bin", "rw");
	
	if (!eventsFile)
		return false;
	
	bool res = fl_fseek(eventsFile, offset, origin) == 0;
	if (res)
	{
		res = fl_fread(data, count, 1, eventsFile) == count;
		if (!res && count > 0)
		{
			fatState = FatStateInit;
		}
	}

	fl_fclose(eventsFile);
	eventsFile = 0;
	return res;
}

bool EventsWrite(long int offset, int origin, unsigned char *data, unsigned int count)
{
	if (fatState != FatStateReady)
		return false;
	
	eventsFile = (FL_FILE*)fl_fopen("/events.bin", "rw");
	
	if (!eventsFile)
		return false;
	
	bool res = fl_fseek(eventsFile, offset, origin) == 0;
	if (res)
	{
		res = fl_fwrite(data, count, 1, eventsFile) == count;
		if (res)
		{
			eventsFileSize += count;
		}
	}

	fl_fclose(eventsFile);
	eventsFile = 0;
	return res;
}

bool EventsSeek(long int offset, int origin, unsigned long &pos)
{
	if (fatState != FatStateReady)
	{
		eventsFileSize = 0;
		return false;
	}
    
	if (eventsFileSize > 0)
	{
		pos = eventsFileSize;
		return true;
	}
	
	eventsFile = (FL_FILE*)fl_fopen("/events.bin", "rw");
	
	if (!eventsFile)
		return false;
	bool res = fl_fseek(eventsFile, offset, origin) == 0;
	unsigned long p = 0;
	fl_fgetpos(eventsFile, &p);
	pos = p;
	eventsFileSize = pos;
	
	fl_fclose(eventsFile);
	eventsFile = 0;
	return res;
}

bool ControllerFilesRead(const char *fileName, long int offset, int origin, unsigned char *data, unsigned int count)
{
	if (fatState != FatStateReady)
		return false;
	
	controllerFile = (FL_FILE*)fl_fopen(fileName, "rw");
	
	if (!controllerFile)
		return false;
	
	bool res = fl_fseek(controllerFile, offset, origin) == 0;
	if (res)
		res = fl_fread(data, count, 1, controllerFile) == count;

	fl_fclose(controllerFile);
	controllerFile = 0;
	return res;
}

bool ControllerFilesWrite(const char *fileName, long int offset, int origin, unsigned char *data, unsigned int count)
{
	if (fatState != FatStateReady)
		return false;
	
	controllerFile = (FL_FILE*)fl_fopen(fileName, "rw");
	
	if (!controllerFile)
		return false;
	
	bool res = fl_fseek(controllerFile, offset, origin) == 0;
	if (res)
		res = fl_fwrite(data, count, 1, controllerFile) == count;

	fl_fclose(controllerFile);
	controllerFile = 0;
	return res;
}

void FileSystemReady(bool &ready)
{
	ready = fatState == FatStateReady;
}

void OscCacheCreateFile(unsigned int fileNumber, unsigned int oscFileSize, bool &result)
{
	if (fatState != FatStateReady)
	{
		return;
	}
	
	char fileName[20];
	snprintf(fileName, sizeof(fileName), "/oscCache/%u", fileNumber);
	
	FL_FILE *file = (FL_FILE*)fl_fopen(fileName, "rw");
	
	if (!file)
	{
		fl_createdirectory("/oscCache/");
		
		file = (FL_FILE*)fl_fopen(fileName, "rw");
	}
	
	if (!file)
	{
		return;
	}
	
	result = true;
	result &= fl_fseek(file, 0, 0) == 0;
	// записываем мусор
	unsigned char data = 0;
	result &= fl_fwrite(&data, oscFileSize, 1, file) == oscFileSize;

	fl_fclose(file);
}

void OscCacheDeleteFile(unsigned int fileNumber, bool &result)
{
	if (fatState != FatStateReady)
	{
		return;
	}
	
	char fileName[20];
	snprintf(fileName, sizeof(fileName), "/oscCache/%u", fileNumber);
	
	result &= fl_remove(fileName) == 0;
}

void OscCacheWriteFile(unsigned int fileNumber, unsigned int offset, unsigned char *data, unsigned int count, bool &result)
{
	if (fatState != FatStateReady)
	{
		return;
	}
	
	char fileName[20];
	snprintf(fileName, sizeof(fileName), "/oscCache/%u", fileNumber);
	
	FL_FILE *file = (FL_FILE*)fl_fopen(fileName, "rw");
	if (!file)
	{
		return;
	}
	
	result = true;
	result &= fl_fseek(file, offset, 0) == 0;
	result &= fl_fwrite(&data, count, 1, file) == count;

	fl_fclose(file);
}

bool OscFilesRead(const char *fileName, long int offset, int origin, unsigned char *data, unsigned int count)
{
	if (fatState != FatStateReady)
		return false;
	
	oscFile = (FL_FILE*)fl_fopen(fileName, "rw");
	
	if (!oscFile)
		return false;
	
	bool res = fl_fseek(oscFile, offset, origin) == 0;
	if (res)
		res = fl_fread(data, count, 1, oscFile) == count;

	fl_fclose(oscFile);
	oscFile = 0;
	return res;
}

bool OscFilesWrite(const char *fileName, long int offset, int origin, unsigned char *data, unsigned int count)
{
	if (fatState != FatStateReady)
		return false;
	
	oscFile = (FL_FILE*)fl_fopen(fileName, "rw");
	
	if (!oscFile)
	{
		fl_createdirectory("/osc");
		
		oscFile = (FL_FILE*)fl_fopen(fileName, "rw");
	}
	
	if (!oscFile)
		return false;
	
	bool res = fl_fseek(oscFile, offset, origin) == 0;
	if (res)
		res = fl_fwrite(data, count, 1, oscFile) == count;

	fl_fclose(oscFile);
	oscFile = 0;
	return res;
}

#endif