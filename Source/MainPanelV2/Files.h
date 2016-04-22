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

unsigned char _oscDataCopyBuffer[200];

void CopyOscData(char *dstFileName, unsigned int fileNumber, int startPos, int endPos)
{
	if (fatState != FatStateReady)
	{
		return;
	}

	int partStart[2] = {0, 0};
	int partCount[2] = {0, 0};
	int partFileNumber[2] = {0, 0};
	
	if (startPos < 0 && endPos < 0xFFFF)
	{
		partCount[0] = -startPos;
		partStart[0] = 0xFFFF - partCount[0];
		partFileNumber[0] = fileNumber - 1;
		partStart[1] = 0;
		partCount[1] = endPos;
		partFileNumber[1] = fileNumber;
	}
	
	if (startPos >= 0 && endPos > 0xFFFF)
	{
		partStart[0] = startPos;
		partCount[0] = 0xFFFF - partStart[0];
		partFileNumber[0] = fileNumber;
		partStart[1] = 0;
		partCount[1] = endPos - 0xFFFF;
		partFileNumber[1] = fileNumber + 1;
	}
	
	if (startPos >= 0 && endPos < 0xFFFF)
	{
		partStart[0] = startPos;
		partCount[0] = endPos - startPos;
		partFileNumber[0] = fileNumber;
	}
	
	for (int p = 0; p < 1; p++)
	{
	
		partStart[p] *= OscFileFormat::OscRecordSize;
		partCount[p] *= OscFileFormat::OscRecordSize;
		
		if (partCount[p] <= 0)
		{
			continue;
		}
		
		char srcFileName[20];
		snprintf(srcFileName, sizeof(srcFileName), "/oscCache/%u", partFileNumber[p]);
		
		FL_FILE *srcFile = (FL_FILE*)fl_fopen(srcFileName, "r");
		FL_FILE *dstFile = (FL_FILE*)fl_fopen(dstFileName, "rw");
		if (!srcFile || !dstFile)
		{
			return;
		}
		
		bool result = true;
		
		int copyCount = partCount[p];
		unsigned int startOffset = partStart[p];
		int i = 0;
		while (i < copyCount)
		{
			int portionSize = copyCount - i;
			if (portionSize > sizeof(_oscDataCopyBuffer))
			{
				portionSize = sizeof(_oscDataCopyBuffer);
			}
			result &= fl_fseek(srcFile, startOffset + i, 0) == 0;
			result &= fl_fread(&_oscDataCopyBuffer, portionSize, 1, srcFile) == portionSize;
			
			result &= fl_fseek(dstFile, 0, 2) == 0;
			result &= fl_fwrite(&_oscDataCopyBuffer, portionSize, 1, dstFile) == portionSize;
			i += portionSize;
			
		}
		
		fl_fclose(srcFile);
		fl_fclose(dstFile);
	}
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