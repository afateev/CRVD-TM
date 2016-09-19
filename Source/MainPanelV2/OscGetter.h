////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		17/01/2013
////////////////////////////////////////////////////////////////////////////////

#ifndef OSCGETTER_H
#define OSCGETTER_H

//#include "MegaLib\MegaLib.h"
#include "OscFileFormat.h"
#include "Controllers.h"

#ifdef USB_STORAGE
#include "ff.h"
#endif

template<class ActiveDriveControllerParams, unsigned int CacheSize>
class OscGetter
{
public:
	enum DriveControllerType
	{
		DriveControllerTypeUnknown = 0,
		DriveControllerTypePrimary,
		DriveControllerTypeSlave
	};
	
	enum StateType
	{
		StateIdle = 0,
		StateActioveControllerChanged,
		StateSendRequest,
		StateWaitResponse,
		StateWriteToFile,
	};
	
	static const unsigned short FramesPerSec = 200;
	static const unsigned char OscBufSize = 20;
	static const unsigned short OscWaitResponseMaxCnt = 10000;
	static const unsigned short OscWaitFileWriteMaxCnt = 350;
	
	static DriveControllerType _controller;	
	
	typedef Rblib::CallbackWrapper<unsigned int &> GetOscCacheFileNumberType;
	typedef Rblib::CallbackWrapper<unsigned int, int, bool &> IsDataLoadedCallbackType;
	typedef Rblib::CallbackWrapper<char *, unsigned int, unsigned int, int, int> AppendOscDataCallbackType;
	typedef Rblib::CallbackWrapper<> OnOscFileAddedCallbackType;
protected:	
	typedef bool FileCallback(const char *fileName, long int offset, int origin, unsigned char *data, unsigned int count);
	static FileCallback *_readFile;
	static FileCallback *_writeFile;
	
	struct OscEventInfo
	{
	public:
		unsigned int Dummy;
		unsigned int FileNumber;
		int Pointer;
	public:
		OscEventInfo()
		{
			Reset();
		}
		
		void Reset()
		{
			Dummy = 0;
			FileNumber = 0;
			Pointer = -1;
		}
	};
	
	static OscEventInfo _oscEventPending[OscFileFormat::OscTypeCount];
public:
	static OscFileFormat::OscFileDescription _oscCache[CacheSize];
	static unsigned long _oscCountInCache;
	static unsigned long _newOscCount;
	static GetOscCacheFileNumberType GetOscCacheFileNumber;
	static IsDataLoadedCallbackType IsDataLoadedCallback;
	static AppendOscDataCallbackType AppendOscDataCallback;
	static OnOscFileAddedCallbackType OnOscFileAddedCallback;
public:
	static void Init(FileCallback *read, FileCallback *write)
	{
		_readFile = read;
		_writeFile = write;
		
		for (int i = 0; i < OscFileFormat::OscTypeCount; i++)
		{
			_oscEventPending[i].Reset();
		}
		
		Reset(false);
	}
	
	static void Reset(bool soft)
	{
		// выключаем экономию
		ActiveDriveControllerParams::SetDoOnlyLowerRegsRequest(false);
	}
	
	static void OnOscEvent(OscFileFormat::OscType oscType, unsigned int pointer)
	{
		//return;
		GetOscCacheFileNumber(_oscEventPending[oscType].FileNumber);
		_oscEventPending[oscType].Pointer = pointer;
	}
	
	static void OscEventPending(bool &pending)
	{
		for (int i = 0; i < OscFileFormat::OscTypeCount; i++)
		{
			if (_oscEventPending[i].Pointer > 0)
			{
				pending = true;
				break;
			}
		}
	}
	
	static void GetEventInfo(int i, unsigned int &file, int &pointer)
	{
		file = _oscEventPending[i].FileNumber;
		pointer = _oscEventPending[i].Pointer;
	}
	
	static bool Run()
	{
		for (int i = 0; i < OscFileFormat::OscTypeCount; i++)
		{
			OscFileFormat::OscType oscType = (OscFileFormat::OscType)i;
			unsigned int fileNumber = _oscEventPending[i].FileNumber;
			int pointer = _oscEventPending[i].Pointer;
			
			// преоритет событий по списку в перечислении
			if (pointer >= 0)
			{
				if (_oscEventPending[i].FileNumber == 65535)
				{
					__no_operation();
				}
				
				int centerPos =  pointer;
				int startPos = 0;
				int endPos = 0;
				
				switch (oscType)
				{
				case OscFileFormat::OscTypeEngineStart:
				case OscFileFormat::OscTypeCheckoutStart:
					{
						startPos = centerPos - 5 * FramesPerSec;
						endPos = centerPos + 25 * FramesPerSec;
					}
					break;
				case OscFileFormat::OscTypeEngineStop:
				case OscFileFormat::OscTypeCheckoutStop:
					{
						startPos = centerPos - 25 * FramesPerSec;
						endPos = centerPos + 5 * FramesPerSec;
					}
					break;
				case OscFileFormat::OscTypeEngineEvent:
					{
						startPos = centerPos - 15 * FramesPerSec;
						endPos = centerPos + 15 * FramesPerSec;
					}
					break;
				}
				
				// нельзя сохранить то, чего быть не может
				// первый файл, нельзя отрицательный старт
				if (startPos < 0 && fileNumber < 1)
				{
					startPos = 0;
				}
				
				bool dataLoaded = false;
				IsDataLoadedCallback(fileNumber, endPos, dataLoaded);
				
				if (dataLoaded)
				{
					// сейчас сохраним, флаг можно снимать
					_oscEventPending[i].Reset();
					
					OscFileFormat::HeaderStruct hdr;
					hdr.Length = endPos - startPos;
					hdr.Center = centerPos - startPos;
					
					if (hdr.Length > 0)
					{
						time_t centerTime = Rblib::Rtc::GetTime();
						// скорректируем реальное время центра
						unsigned short curOscPos = ActiveDriveControllerParams::GetOscCurPos();
						unsigned short elapsed = MarkersDelta(centerPos, curOscPos);
						elapsed /= FramesPerSec;
							
						if (centerTime > elapsed)
						{
							centerTime -= elapsed;
						}
						
						char fileName[50];
						OscFileFormat::FormatFileName(fileName, sizeof(fileName), centerTime, oscType);
								
						bool res = _writeFile(fileName, 0, 0, (unsigned char *)&hdr, sizeof(OscFileFormat::HeaderStruct));
						
						if (res)
						{
							OscFileFormat::AnalogValuesStruct analogValues;
							analogValues.Ust = ActiveDriveControllerParams::GetRegValue(ActiveDriveControllerParams::RegStatorVoltageMaxDisplay);
							analogValues.Ist = ActiveDriveControllerParams::GetRegValue(ActiveDriveControllerParams::RegStatorCurrentMaxDisplay);
							analogValues.Urot = ActiveDriveControllerParams::GetRegValue(ActiveDriveControllerParams::RegRotorVoltageMaxDisplay);
							analogValues.Irot = ActiveDriveControllerParams::GetRegValue(ActiveDriveControllerParams::RegRotorCurrentMaxDisplay);
							
							res = _writeFile(fileName, sizeof(OscFileFormat::HeaderStruct), 0, (unsigned char *)&analogValues, sizeof(OscFileFormat::AnalogValuesStruct));
						}
						
						if (res)
						{
							AppendOscDataCallback(fileName, fileNumber, sizeof(OscFileFormat::HeaderStruct) + sizeof(OscFileFormat::AnalogValuesStruct), startPos, endPos);
							OnOscFileAddedCallback();
						}
					}
				}
				
				break;
			}
		}

		return true;
	}
	
	static unsigned long GetOscCount()
	{
#ifdef SD_STORAGE
		unsigned int res = 0;
		
		FL_DIR dirstat;
		if (fl_opendir("/osc", &dirstat))
		{
			struct fs_dir_ent dirent;
	
			while (fl_readdir(&dirstat, &dirent) == 0)
			{
				if (!dirent.is_dir)
				{
					res++;
					//FAT_PRINTF(("%d - %s <DIR> (0x%08lx)\r\n",++filenumber, dirent.filename, dirent.cluster));
				}
			}
	
			fl_closedir(&dirstat);
		}
		
		return res;
#endif
#ifdef USB_STORAGE
		FRESULT res;
		DIR dir;
		static FILINFO fno;
		static char lfn[_MAX_LFN + 1];
		fno.lfname = lfn;
		fno.lfsize = sizeof(lfn);
		int count = 0;


		res = f_opendir(&dir, "osc");                       /* Open the directory */
		if (res == FR_OK)
		{
			for (;;)
			{
				res = f_readdir(&dir, &fno);                   /* Read a directory item */
				if (res != FR_OK || fno.fname[0] == 0)
				{
					break;  /* Break on error or end of dir */
				}
				if (!(fno.fattrib & AM_DIR))
				{                    /* It is a directory */
					count++;
				}
			}
			f_closedir(&dir);
		}
		
		return count;
#endif
	}
	
	static void Cache(unsigned long lastOscNumber, unsigned long count)
	{
		/*
		if (!_seekFile)
			return;
		if (!_writeFile)
			return;
		
		if (!_readFile)
			return;
		*/
		/*
		if (!_seekFile(0, 2, pos)) // в конец
			return;
		*/
		long last = lastOscNumber;
		if (last > count)
			last = count;
		long first = last;
		first -= CacheSize - 1;
		if (first < 1)
			first = 1;
		
		unsigned int fileNumber = 0;
		unsigned int writePos = 0;
		
		_oscCountInCache = last - first + 1;
#ifdef SD_STORAGE		
		FL_DIR dirstat;
		if (fl_opendir("/osc", &dirstat))
		{
			struct fs_dir_ent dirent;
	
			while (fl_readdir(&dirstat, &dirent) == 0)
			{
				if (!dirent.is_dir)
				{
					fileNumber++;
					if (fileNumber >= first)
					{
						int i = 0;
						for (i = 0; i < OscFileFormat::FileNameSize; i++)
						{
							_oscCache[writePos].FileName[i] = dirent.filename[i];
							if (_oscCache[writePos].FileName[i] == 0)
							{
								break;
							}
						}
						
						_oscCache[writePos].FileName[i] = 0;
						writePos++;
						
						if (fileNumber >= last)
						{
							break;
						}
						
						if (writePos >= CacheSize)
						{
							break;
						}
					}
					//FAT_PRINTF(("%d - %s <DIR> (0x%08lx)\r\n",++filenumber, dirent.filename, dirent.cluster));
				}
			}
	
			fl_closedir(&dirstat);
		}
#endif
#ifdef USB_STORAGE
			FRESULT res;
			DIR dir;
			static FILINFO fno;
			static char lfn[_MAX_LFN + 1];
			fno.lfname = lfn;
			fno.lfsize = sizeof(lfn);

			res = f_opendir(&dir, "osc");                       /* Open the directory */
			if (res == FR_OK)
			{
				for (;;)
				{
					res = f_readdir(&dir, &fno);                   /* Read a directory item */
					if (res != FR_OK || fno.fname[0] == 0)
					{
						break;  /* Break on error or end of dir */
					}
					if (!(fno.fattrib & AM_DIR))
					{                    /* It is a directory */
						fileNumber++;
						
						if (fileNumber >= first)
						{
							int i = 0;
							for (i = 0; i < OscFileFormat::FileNameSize; i++)
							{
								// длинное имя передается в буфере, который мы указали при инициализации fno
								_oscCache[writePos].FileName[i] = lfn[i];
								if (_oscCache[writePos].FileName[i] == 0)
								{
									break;
								}
							}
							
							_oscCache[writePos].FileName[i] = 0;
							writePos++;
							
							if (fileNumber >= last)
							{
								break;
							}
							
							if (writePos >= CacheSize)
							{
								break;
							}
						}
					}
				}
				f_closedir(&dir);
			}
#endif
		
		/*
		if (!_readFile((first - 1) * sizeof(Event::EventData), 0, (unsigned char *)_eventsCache, _eventsCountInCache * sizeof(Event::EventData)))
			_eventsCountInCache = 0;
		*/
	}
	
	static short GetProgress()
	{
		return 100;
		/*
		float remains = MarkersDelta(_osc.CurPos, _osc.EndPos);
		float total = MarkersDelta(_osc.StartPos, _osc.EndPos);
		float progress = 100 - remains / total * 100;
		return (short)progress;*/
	}
protected:
	static bool TestActiveController()
	{
		bool res = false;
        DriveControllerType newValue = DriveControllerTypeUnknown;
		
		if (ActiveDriveControllerParams::IsPrimaryActive())
		{
			newValue = DriveControllerTypePrimary;
		}
		else
		{
			newValue = DriveControllerTypeSlave;
		}
		
		if (_controller != newValue)
		{
			OnActiveControllerChanged(newValue);
            res = true;
		}
        
        return res;
	}
	
	static unsigned short MarkersDelta(unsigned short from, unsigned short to)
	{
		unsigned short res = 0;
		if (from <= to)
		{
			res = to - from;
		}
		else
		{
			res = 0xFFFF - from;
			res += to;
		}
		return res;
	}
	
	static int min(int v1, int v2)
	{
		if (v1 < v2)
		{
			return v1;
		}
		
		return v2;
	}
};

template<class ActiveDriveControllerParams, unsigned int CacheSize>
OscGetter<ActiveDriveControllerParams, CacheSize>::DriveControllerType OscGetter<ActiveDriveControllerParams, CacheSize>::_controller = OscGetter<ActiveDriveControllerParams, CacheSize>::DriveControllerTypeUnknown;

template<class ActiveDriveControllerParams, unsigned int CacheSize>
OscGetter<ActiveDriveControllerParams, CacheSize>::FileCallback *OscGetter<ActiveDriveControllerParams, CacheSize>::_readFile = 0;

template<class ActiveDriveControllerParams, unsigned int CacheSize>
OscGetter<ActiveDriveControllerParams, CacheSize>::FileCallback *OscGetter<ActiveDriveControllerParams, CacheSize>::_writeFile = 0;

template<class ActiveDriveControllerParams, unsigned int CacheSize>
OscFileFormat::OscFileDescription OscGetter<ActiveDriveControllerParams, CacheSize>::_oscCache[CacheSize];

template<class ActiveDriveControllerParams, unsigned int CacheSize>
unsigned long OscGetter<ActiveDriveControllerParams, CacheSize>::_oscCountInCache = 0;

template<class ActiveDriveControllerParams, unsigned int CacheSize>
unsigned long OscGetter<ActiveDriveControllerParams, CacheSize>::_newOscCount = 0;

template<class ActiveDriveControllerParams, unsigned int CacheSize>
OscGetter<ActiveDriveControllerParams, CacheSize>::OscEventInfo OscGetter<ActiveDriveControllerParams, CacheSize>::_oscEventPending[];

template<class ActiveDriveControllerParams, unsigned int CacheSize>
OscGetter<ActiveDriveControllerParams, CacheSize>::GetOscCacheFileNumberType OscGetter<ActiveDriveControllerParams, CacheSize>::GetOscCacheFileNumber;

template<class ActiveDriveControllerParams, unsigned int CacheSize>
OscGetter<ActiveDriveControllerParams, CacheSize>::IsDataLoadedCallbackType OscGetter<ActiveDriveControllerParams, CacheSize>::IsDataLoadedCallback;

template<class ActiveDriveControllerParams, unsigned int CacheSize>
OscGetter<ActiveDriveControllerParams, CacheSize>::AppendOscDataCallbackType OscGetter<ActiveDriveControllerParams, CacheSize>::AppendOscDataCallback;

template<class ActiveDriveControllerParams, unsigned int CacheSize>
OscGetter<ActiveDriveControllerParams, CacheSize>::OnOscFileAddedCallbackType OscGetter<ActiveDriveControllerParams, CacheSize>::OnOscFileAddedCallback;

typedef OscGetter<ActiveDriveControllerParams, 13> OscGet;

#endif