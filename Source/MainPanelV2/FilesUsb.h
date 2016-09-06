#ifndef FILES_USB_H
#define FILES_USB_H

#include "Drivers.h"

enum FatState
{
	FatStateInit = 0,
	FatStateError,
	FatStateConnectionDetected,
	FatStateDisconnectionDetected,
	FatStateDisconnected,
    FatStateConnected,
	FatStateReady
};

FatState fatState = FatStateInit;

bool FileSeek(FIL *file, long int offset, int origin)
{
	bool res = false;
	
	switch(origin)
	{
	case 0: // от начала
		{
			res = f_lseek(file, offset) == FR_OK;
		}
		break;
	case 1: // от текущей позиции
		{
		}
		break;
	case 2: // от конца
		{
			res = f_lseek(file, f_size(file) - offset) == FR_OK;
		}
		break;
	}
	
	return res;
}

bool EventsRead(long int offset, int origin, unsigned char *data, unsigned int count)
{
	if (fatState != FatStateReady)
		return false;
	
	FIL file;
	
	bool res = true;
	res &= f_open(&file, "events.bin", FA_READ | FA_WRITE | FA_OPEN_ALWAYS) == FR_OK;
	if (res)
	{
		res = FileSeek(&file, offset, origin);
		
		if (res)
		{
			unsigned int bytesread = 0;
			res &= f_read(&file, data, count, &bytesread) == FR_OK;
			res &= bytesread == count;
		}
		
		f_close(&file);
	}
	
	return res;
}

bool EventsWrite(long int offset, int origin, unsigned char *data, unsigned int count)
{
	if (fatState != FatStateReady)
		return false;
	
	FIL file;
	
	bool res = true;
	res &= f_open(&file, "events.bin", FA_READ | FA_WRITE | FA_OPEN_ALWAYS) == FR_OK;
	if (res)
	{
		res = FileSeek(&file, offset, origin);
		
		if (res)
		{
			unsigned int byteswrite = 0;
			res &= f_write (&file, data, count, &byteswrite) == FR_OK;
			res &= byteswrite == count;
		}
		
		f_close(&file);
	}
	
	return res;
}

bool EventsSeek(long int offset, int origin, unsigned long &pos)
{
	if (fatState != FatStateReady)
		return false;
	
	FIL file;
	
	bool res = true;
	res &= f_open(&file, "events.bin", FA_READ | FA_WRITE | FA_OPEN_ALWAYS) == FR_OK;
	if (res)
	{
		res = FileSeek(&file, offset, origin);
		
		if (res)
		{
			pos = f_tell(&file);
		}
		
		f_close(&file);
	}
	
	return res;
}

bool ControllerFilesRead(const char *fileName, long int offset, int origin, unsigned char *data, unsigned int count)
{
	if (fatState != FatStateReady)
		return false;
	
	FIL file;
	
	bool res = true;
	res &= f_open(&file, fileName, FA_READ | FA_WRITE | FA_OPEN_ALWAYS) == FR_OK;
	if (res)
	{
		res = FileSeek(&file, offset, origin);
		
		if (res)
		{
			unsigned int bytesread = 0;
			res &= f_read(&file, data, count, &bytesread) == FR_OK;
			res &= bytesread == count;
		}
		
		f_close(&file);
	}
	
	return res;
}

bool ControllerFilesWrite(const char *fileName, long int offset, int origin, unsigned char *data, unsigned int count)
{
	if (fatState != FatStateReady)
		return false;
	
	FIL file;
	
	bool res = true;
	res &= f_open(&file, fileName, FA_READ | FA_WRITE | FA_OPEN_ALWAYS) == FR_OK;
	if (res)
	{
		res = FileSeek(&file, offset, origin);
		
		if (res)
		{
			unsigned int byteswrite = 0;
			res &= f_write (&file, data, count, &byteswrite) == FR_OK;
			res &= byteswrite == count;
		}
		
		f_close(&file);
	}
	
	return res;
}

void FileSystemReady(bool &ready)
{
	ready = fatState == FatStateReady;
}

void OscCacheCreateFile(unsigned int fileNumber, unsigned int oscFileSize, bool &result)
{
	if (fatState != FatStateReady)
		return;
	
	FILINFO fno;
	static char lfn[_MAX_LFN + 1];
	fno.lfname = lfn;
	fno.lfsize = sizeof(lfn);
	
	if (f_stat("oscCache", &fno) != FR_OK)
	{
		f_mkdir("oscCache");
	}
	
	char fileName[20];
	snprintf(fileName, sizeof(fileName), "oscCache/%u", fileNumber);
	
	FIL file;
	
	bool res = true;
	res &= f_open(&file, fileName, FA_READ | FA_WRITE | FA_CREATE_ALWAYS) == FR_OK;
	if (res)
	{
		f_close(&file);
		result = true;
	}
}

void OscCacheDeleteFile(unsigned int fileNumber, bool &result)
{
	if (fatState != FatStateReady)
		return;
	
	char fileName[20];
	snprintf(fileName, sizeof(fileName), "oscCache/%u", fileNumber);
	
	result &= f_unlink (fileName) == FR_OK;
}

void OscCacheWriteFile(unsigned int fileNumber, unsigned int offset, unsigned char *data, unsigned int count, bool &result)
{
	if (fatState != FatStateReady)
		return;
	
	char fileName[20];
	snprintf(fileName, sizeof(fileName), "oscCache/%u", fileNumber);
	
	FIL file;
	
	result = f_open(&file, fileName, FA_READ | FA_WRITE | FA_OPEN_ALWAYS) == FR_OK;
	if (result)
	{
		result = FileSeek(&file, offset, 0);
		
		if (result)
		{
			unsigned int byteswrite = 0;
			result &= f_write (&file, data, count, &byteswrite) == FR_OK;
			result &= byteswrite == count;
		}
		
		f_close(&file);
	}
}

unsigned char _oscDataCopyBuffer[256];

void CopyOscData(char *dstFileName, unsigned int fileNumber, unsigned int dataOffset, int startPos, int endPos)
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
	
	unsigned int writeOffset = 0;
	
	for (int p = 0; p < 2; p++)
	{
	
		partStart[p] *= OscFileFormat::OscRecordSize;
		partCount[p] *= OscFileFormat::OscRecordSize;
		
		if (partCount[p] <= 0)
		{
			continue;
		}
		
		char srcFileName[20];
		snprintf(srcFileName, sizeof(srcFileName), "oscCache/%u", partFileNumber[p]);
		
		FIL srcFile;
		bool res = f_open(&srcFile, srcFileName, FA_READ) == FR_OK;

		if (!res)
		{
			return;
		}
		
		bool result = true;
		
		FIL dstFile;
		result &= f_open(&dstFile, dstFileName, FA_READ | FA_WRITE | FA_OPEN_ALWAYS) == FR_OK;
		
		int copyCount = partCount[p];
		unsigned int startOffset = partStart[p];
		int i = 0;
		/*
		for (int i = 0; i < sizeof(_oscDataCopyBuffer); i++)
		{
			_oscDataCopyBuffer[i] = i;
		}*/
		
		while (i < copyCount)
		{
			int portionSize = copyCount - i;
			if (portionSize > sizeof(_oscDataCopyBuffer))
			{
				portionSize = sizeof(_oscDataCopyBuffer);
			}
			result &= f_lseek(&srcFile, startOffset + i) == FR_OK;
			unsigned int readed = 0;
			result &= f_read(&srcFile, _oscDataCopyBuffer, portionSize, &readed) == FR_OK;
			result &= readed == portionSize;
			
			//FIL dstFile;
			//result &= f_open(&dstFile, dstFileName, FA_READ | FA_WRITE | FA_OPEN_ALWAYS) == FR_OK;
			
			result &= f_lseek(&dstFile, dataOffset + writeOffset) == FR_OK;
			unsigned int writed = 0;
			result &= f_write(&dstFile, _oscDataCopyBuffer, portionSize, &writed);
			result &= writed == portionSize;
			//f_close(&dstFile);
			i += portionSize;
			writeOffset += portionSize;
		}
		
		f_close(&dstFile);
		f_close(&srcFile);
	}
}

bool OscFilesRead(const char *fileName, long int offset, int origin, unsigned char *data, unsigned int count)
{
	if (fatState != FatStateReady)
		return false;
	
	FIL file;
	
	bool res = true;
	res &= f_open(&file, fileName, FA_READ | FA_WRITE | FA_OPEN_ALWAYS) == FR_OK;
	if (res)
	{
		res = FileSeek(&file, offset, origin);
		
		if (res)
		{
			unsigned int bytesread = 0;
			res &= f_read(&file, data, count, &bytesread) == FR_OK;
			res &= bytesread == count;
		}
		
		f_close(&file);
	}
	
	return res;
}

bool OscFilesWrite(const char *fileName, long int offset, int origin, unsigned char *data, unsigned int count)
{
	if (fatState != FatStateReady)
		return false;
	
	FILINFO fno;
	static char lfn[_MAX_LFN + 1];
	fno.lfname = lfn;
	fno.lfsize = sizeof(lfn);
	
	if (f_stat("osc", &fno) != FR_OK)
	{
		f_mkdir("osc");
	}
	
	FIL file;
	
	bool res = true;
	res &= f_open(&file, fileName, FA_READ | FA_WRITE | FA_OPEN_ALWAYS) == FR_OK;
	if (res)
	{
		res = FileSeek(&file, offset, origin);
		
		if (res)
		{
			unsigned int byteswrite = 0;
			res &= f_write (&file, data, count, &byteswrite) == FR_OK;
			res &= byteswrite == count;
		}
		
		f_close(&file);
	}
	
	return res;
}

#endif