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
	struct OscCaptureInfo
	{
		StateType State; 
		unsigned short EngineStartPos;
		unsigned short EngineStopPos;
		unsigned short EngineEventPos;
		unsigned short ChechoutStartPos;
		unsigned short ChechoutStopPos;
		
		OscFileFormat::OscType Type;
		time_t CenterTime;
		unsigned short CenterPos;
		unsigned short StartPos;
		unsigned short EndPos;
		unsigned short CurPos;
		bool FileHeaderCreated;
		unsigned char RecordsLoaded;
		OscFileFormat::DataStruct Records[OscBufSize];
		unsigned short OscWaitCnt;
		char LastFileName[50];
	};
	
	typedef Rblib::CallbackWrapper<char *> AppendOscDataCallbackType;
protected:	
	static OscCaptureInfo _osc;
	
	typedef bool FileCallback(const char *fileName, long int offset, int origin, unsigned char *data, unsigned int count);
	static FileCallback *_readFile;
	static FileCallback *_writeFile;
	
	static bool _saveOsc;
public:
	static OscFileFormat::OscFileDescription _oscCache[CacheSize];
	static unsigned long _oscCountInCache;
	static unsigned long _newOscCount;
	static AppendOscDataCallbackType AppendOscDataCallback;
public:
	static void Init(FileCallback *read, FileCallback *write)
	{
		_readFile = read;
		_writeFile = write;
		
		_saveOsc = false;
		
		Reset(false);
	}
	
	static void Reset(bool soft)
	{
		unsigned short engineStartPos = _osc.EngineStartPos;
		unsigned short engineStopPos = _osc.EngineStopPos;;
		unsigned short engineEventPos = _osc.EngineEventPos;;
		unsigned short ehechoutStartPos = _osc.ChechoutStartPos;;
		unsigned short ehechoutStopPos = _osc.ChechoutStopPos;;
		
		memset(&_osc, 0, sizeof(OscCaptureInfo));
		if (soft)
		{
			_osc.EngineStartPos = engineStartPos;
			_osc.EngineStopPos = engineStopPos;
			_osc.EngineEventPos = engineEventPos;
			_osc.ChechoutStartPos = ehechoutStartPos;
			_osc.ChechoutStopPos = ehechoutStopPos;
		}
		else
		{
			_osc.EngineStartPos = 0xFFFF;
			_osc.EngineStopPos = 0xFFFF;
			_osc.EngineEventPos = 0xFFFF;
			_osc.ChechoutStartPos = 0xFFFF;
			_osc.ChechoutStopPos = 0xFFFF;
		}
		_osc.FileHeaderCreated = false;
		_osc.RecordsLoaded = 0;
		
		// выключаем экономию
		ActiveDriveControllerParams::SetDoOnlyLowerRegsRequest(false);
	}
	
	static void OnOscEvent(OscFileFormat::OscType oscType, unsigned int pointer)
	{
		switch (oscType)
		{
		case OscFileFormat::OscTypeEngineStop:
			{
				_saveOsc = true;
			}
			break;
		}
	}
	
	static bool Run()
	{
		if (_saveOsc)
		{
			_saveOsc = false;
			
			OscFileFormat::HeaderStruct hdr;
			hdr.Length = 30 * FramesPerSec;		
			hdr.Center = 25 * FramesPerSec;		
			
			char fileName[50];
			OscFileFormat::FormatFileName(fileName, sizeof(fileName), Rtc::GetTime(), OscFileFormat::OscTypeEngineStop);
					
			bool res = _writeFile(fileName, 0, 0, (unsigned char *)&hdr, sizeof(OscFileFormat::HeaderStruct));
			
			if (res)
			{
				OscFileFormat::AnalogValuesStruct analogValues;
				analogValues.Ust = ActiveDriveControllerParams::GetRegValue(ActiveDriveControllerParams::RegStatorVoltageMaxDisplay);
				analogValues.Ist = ActiveDriveControllerParams::GetRegValue(ActiveDriveControllerParams::RegStatorCurrentMaxDisplay);
				analogValues.Urot = ActiveDriveControllerParams::GetRegValue(ActiveDriveControllerParams::RegRotorVoltageMaxDisplay);
				analogValues.Irot = ActiveDriveControllerParams::GetRegValue(ActiveDriveControllerParams::RegRotorCurrentMaxDisplay);
				
				res = _writeFile(fileName, sizeof(OscFileFormat::HeaderStruct), 0, (unsigned char *)&analogValues, sizeof(OscFileFormat::AnalogValuesStruct));
				
				if (res)
				{
					_osc.FileHeaderCreated = true;
				}
			}
			
			if (res)
			{
				AppendOscDataCallback(fileName);
			}
		}
		
		return true;
	}
	
	static bool Run1()
	{
		TestActiveController();
		
		switch(_osc.State)
		{
		case StateIdle:
			{
				if (ActiveDriveControllerParams::IsWaitRegistersResponse())
				{
					break;
				}
				
				// нет занчений регистров, без которых мы не сможем записать файл осциллограммы, не стоит и начинать скачивание
				if (!ActiveDriveControllerParams::HasRegValue(ActiveDriveControllerParams::RegStatorVoltageMaxDisplay))
				{
					break;
				}
				
				static int shots = 0;
				//if (shots > 3)
				//	break;
				if (_controller != DriveControllerTypeUnknown)
				{
					if (_osc.State != StateSendRequest)
					{
						if (_osc.EngineStartPos != ActiveDriveControllerParams::GetOscEngineStart())
						{
							_osc.EngineStartPos = ActiveDriveControllerParams::GetOscEngineStart();
							
							if (_osc.EngineStartPos != 0xFFFF)
							{
								_osc.Type = OscFileFormat::OscTypeEngineStart;
								_osc.CenterPos = _osc.EngineStartPos;
								_osc.StartPos = _osc.CenterPos - 5 * FramesPerSec;
								_osc.EndPos = _osc.CenterPos + 25 * FramesPerSec;
								_osc.State = StateSendRequest;
								
								shots++;
							}
						}
					}
					
					if (_osc.State != StateSendRequest)
					{
						if (_osc.EngineStopPos != ActiveDriveControllerParams::GetOscEngineStop())
						{
							_osc.EngineStopPos = ActiveDriveControllerParams::GetOscEngineStop();
							
							if (_osc.EngineStopPos != 0xFFFF)
							{
								_osc.Type = OscFileFormat::OscTypeEngineStop;
								_osc.CenterPos = _osc.EngineStopPos;
								_osc.StartPos = _osc.CenterPos - 25 * FramesPerSec;
								_osc.EndPos = _osc.CenterPos + 5 * FramesPerSec;
								_osc.State = StateSendRequest;
								
								shots++;
							}
						}
					}
					
					if (_osc.State != StateSendRequest)
					{
						if (_osc.EngineEventPos != ActiveDriveControllerParams::GetOscEngineEvent())
						{
							_osc.EngineEventPos = ActiveDriveControllerParams::GetOscEngineEvent();
							
							if (_osc.EngineEventPos != 0xFFFF)
							{
								_osc.Type = OscFileFormat::OscTypeEngineEvent;
								_osc.CenterPos = _osc.EngineEventPos;
								_osc.StartPos = _osc.CenterPos - 15 * FramesPerSec;
								_osc.EndPos = _osc.CenterPos + 15 * FramesPerSec;
								_osc.State = StateSendRequest;
								
								shots++;
							}
						}
					}
					
					if (_osc.State != StateSendRequest)
					{
						if (_osc.ChechoutStartPos != ActiveDriveControllerParams::GetOscCheckoutStart())
						{
							_osc.ChechoutStartPos = ActiveDriveControllerParams::GetOscCheckoutStart();
							
							if (_osc.ChechoutStartPos != 0xFFFF)
							{
								_osc.Type = OscFileFormat::OscTypeCheckoutStart;
								_osc.CenterPos = _osc.ChechoutStartPos;
								_osc.StartPos = _osc.CenterPos - 5 * FramesPerSec;
								_osc.EndPos = _osc.CenterPos + 25 * FramesPerSec;
								_osc.State = StateSendRequest;
								
								shots++;
							}
						}
					}
					
					if (_osc.State != StateSendRequest)
					{
						if (_osc.ChechoutStopPos != ActiveDriveControllerParams::GetOscCheckoutStop())
						{
							_osc.ChechoutStopPos = ActiveDriveControllerParams::GetOscCheckoutStop();
							
							if (_osc.ChechoutStopPos != 0xFFFF)
							{
								_osc.Type = OscFileFormat::OscTypeCheckoutStop;
								_osc.CenterPos = _osc.ChechoutStopPos;
								_osc.StartPos = _osc.CenterPos - 25 * FramesPerSec;
								_osc.EndPos = _osc.CenterPos + 5 * FramesPerSec;
								_osc.State = StateSendRequest;
								
								shots++;
							}
						}
					}
					
					// если переходим в состояние отправки запроса, значит начинаем закачку
					if (_osc.State == StateSendRequest)
					{
						_osc.CurPos = _osc.StartPos;
						_osc.RecordsLoaded = 0;
						_osc.CenterTime = Rtc::GetTime();
						// скорректируем реальное время центра
						unsigned short curOscPos = ActiveDriveControllerParams::GetOscCurPos();
						unsigned short elapsed = MarkersDelta(_osc.CenterPos, curOscPos);
						elapsed /= FramesPerSec;
							
						if (_osc.CenterTime > elapsed)
						{
							_osc.CenterTime -= elapsed;
						}
						
						// начнём запись файла
						WriteFileHeader(0);
					}
				}
			}
			break;
		case StateActioveControllerChanged:
			{
				Reset(false);
			}
			break;
		case StateSendRequest:
			{
				/*
                bool needBreak = false;
                while(ActiveDriveControllerParams::IsWaitRegistersResponse() && !needBreak)
				{
                    needBreak = TestActiveController();
                }
                
                if (needBreak)
                {
                    break;
                }
                */
				/*
				if (ActiveDriveControllerParams::IsWaitRegistersResponse())
				{
					break;
				}
				*/
				unsigned int startMarker = 0xFFFF;
				switch(_osc.Type)
				{
				case OscFileFormat::OscTypeEngineStart:
					{
						startMarker = ActiveDriveControllerParams::GetOscEngineStart();
					}
					break; 
				case OscFileFormat::OscTypeEngineStop:
					{
						startMarker = ActiveDriveControllerParams::GetOscEngineStop();
					}
					break;
				case OscFileFormat::OscTypeEngineEvent:
					{
						startMarker = ActiveDriveControllerParams::GetOscEngineEvent();
					}
					break;
				case OscFileFormat::OscTypeCheckoutStart:
					{
						startMarker = ActiveDriveControllerParams::GetOscCheckoutStart();
					}
					break; 
				case OscFileFormat::OscTypeCheckoutStop:
					{
						startMarker = ActiveDriveControllerParams::GetOscCheckoutStop();
					}
					break;
				}
				/*
				// если вдруг осциллограмма начала затираться, значит мы не успели её выкачать
				if (startMarker == 0xFFFF)
				{
					// ничего не остается, кроме как прервать скачку
					Reset();
					break;
				}
				*/
				unsigned short remains = MarkersDelta(_osc.CurPos, _osc.EndPos);
				unsigned char emptySpaseInBuffer = OscBufSize - _osc.RecordsLoaded;
				
				if (remains > 0)
				{
					unsigned short canLoad = remains;
					
					if (canLoad > emptySpaseInBuffer)
					{
						canLoad = emptySpaseInBuffer;
					}
					
					if (canLoad > 0)
					{
						if (ActiveDriveControllerParams::OscRequest((unsigned char *)&_osc.Records[_osc.RecordsLoaded], _osc.CurPos, canLoad))
						{
							_osc.State = StateWaitResponse;
							_osc.OscWaitCnt = 0;
						}
					}
				}
				else
				{
					Reset(true);
				}
			}
			break;
		case StateWaitResponse:
			{
				/*
                bool needBreak = false;
                while(ActiveDriveControllerParams::OscRequestWait() && !needBreak)
				{
                    needBreak = TestActiveController();
                }
                
                if (needBreak)
                {
                    break;
                }
                */
				if (!ActiveDriveControllerParams::OscRequestWait())
				{
					unsigned int startPoint = 0;
					unsigned int pointsCount = 0;
					
					if (ActiveDriveControllerParams::OscResponseReady(startPoint, pointsCount))
					{
						for (unsigned char i = _osc.RecordsLoaded; i < _osc.RecordsLoaded + pointsCount; i++)
						{
							_osc.Records[i].SwapBytes();
						}
						
						if (startPoint == _osc.CurPos)
						{
							_osc.RecordsLoaded += pointsCount;
							_osc.CurPos += pointsCount;
							_osc.State = StateWriteToFile;
							_osc.OscWaitCnt = 0;
						}
						else
						{
							_osc.State = StateSendRequest;
						}
						break;
					}
					else
					{
						_osc.OscWaitCnt++;
						if (_osc.OscWaitCnt >= OscWaitResponseMaxCnt)
						{
							_osc.State = StateSendRequest;
							break;
						}
					}
				}
			}
			break;
		case StateWriteToFile:
			{
				if (WriteDataToFile(_osc.Records, _osc.RecordsLoaded))
				{
					_osc.RecordsLoaded = 0;
					_osc.State = StateSendRequest;
				}
				else
				{
					_osc.OscWaitCnt++;
					if (_osc.OscWaitCnt >= OscWaitFileWriteMaxCnt)
					{
						Reset(true);
					}
				}
			}
			break;
		}
		
		return _osc.State == StateIdle;
	}
	
	static unsigned long GetOscCount()
	{
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
		
		/*
		if (!_readFile((first - 1) * sizeof(Event::EventData), 0, (unsigned char *)_eventsCache, _eventsCountInCache * sizeof(Event::EventData)))
			_eventsCountInCache = 0;
		*/
	}
	
	static char *GetLastFileName()
	{
		return _osc.LastFileName;
	}
	
	static short GetProgress()
	{
		if (_osc.State == StateIdle)
		{
			return 100;
		}
		
		float remains = MarkersDelta(_osc.CurPos, _osc.EndPos);
		float total = MarkersDelta(_osc.StartPos, _osc.EndPos);
		float progress = 100 - remains / total * 100;
		return (short)progress;
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
	
	static void OnActiveControllerChanged(DriveControllerType newValue)
	{
		_osc.State = StateActioveControllerChanged;
		_controller = newValue;
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
	
	static bool WriteFileHeader(OscFileFormat::HeaderStruct *header)
	{
		OscFileFormat::HeaderStruct *hdr = header;
		
		OscFileFormat::HeaderStruct defaultHeader;
		defaultHeader.Center = MarkersDelta(_osc.StartPos, _osc.CenterPos);		
		
		if (hdr == 0 && !_osc.FileHeaderCreated)
		{
			hdr = &defaultHeader;
		}
		
		if (hdr == 0)
		{
			return false;
		}
		
		char fileName[50];
		OscFileFormat::FormatFileName(fileName, sizeof(fileName), _osc.CenterTime, _osc.Type);
		memcpy(_osc.LastFileName, fileName, min(sizeof(fileName), sizeof(_osc.LastFileName)));
				
		bool res = _writeFile(fileName, 0, 0, (unsigned char *)hdr, sizeof(OscFileFormat::HeaderStruct));
		
		if (res && !_osc.FileHeaderCreated)
		{
			OscFileFormat::AnalogValuesStruct analogValues;
			analogValues.Ust = ActiveDriveControllerParams::GetRegValue(ActiveDriveControllerParams::RegStatorVoltageMaxDisplay);
			analogValues.Ist = ActiveDriveControllerParams::GetRegValue(ActiveDriveControllerParams::RegStatorCurrentMaxDisplay);
			analogValues.Urot = ActiveDriveControllerParams::GetRegValue(ActiveDriveControllerParams::RegRotorVoltageMaxDisplay);
			analogValues.Irot = ActiveDriveControllerParams::GetRegValue(ActiveDriveControllerParams::RegRotorCurrentMaxDisplay);
			
			res = _writeFile(fileName, sizeof(OscFileFormat::HeaderStruct), 0, (unsigned char *)&analogValues, sizeof(OscFileFormat::AnalogValuesStruct));
			
			if (res)
			{
				_osc.FileHeaderCreated = true;
			}
		}
		
		return res;
	}
	
	static bool WriteDataToFile(OscFileFormat::DataStruct *records, unsigned short RecordsCount)
	{
		if (records == 0 || RecordsCount == 0)
		{
			return false;
		}
		
		bool res = _osc.FileHeaderCreated;
		
		if (!res)
		{
			res = WriteFileHeader(0);
		}
		
		if (res)
		{
			// для экономии выключим запрос лишних регистров
			ActiveDriveControllerParams::SetDoOnlyLowerRegsRequest(true);
			
			char fileName[50];
			OscFileFormat::FormatFileName(fileName, sizeof(fileName), _osc.CenterTime, _osc.Type);
			memcpy(_osc.LastFileName, fileName, min(sizeof(fileName), sizeof(_osc.LastFileName)));
			
			OscFileFormat::HeaderStruct header;
			
			res = _readFile(fileName, 0, 0, (unsigned char *)&header, sizeof(OscFileFormat::HeaderStruct));
			
			if (res)
			{
				res = header.Version == 0x0200;
				
				if (res)
				{
					res = _writeFile(fileName, 0, 2, (unsigned char *)records, sizeof(OscFileFormat::DataStruct) * RecordsCount);
					if (res)
					{
						header.Length += RecordsCount;
						res = WriteFileHeader(&header);
					}
				}
			}
			
			
		}
		
		return res;
	}
};

template<class ActiveDriveControllerParams, unsigned int CacheSize>
OscGetter<ActiveDriveControllerParams, CacheSize>::DriveControllerType OscGetter<ActiveDriveControllerParams, CacheSize>::_controller = OscGetter<ActiveDriveControllerParams, CacheSize>::DriveControllerTypeUnknown;

template<class ActiveDriveControllerParams, unsigned int CacheSize>
OscGetter<ActiveDriveControllerParams, CacheSize>::OscCaptureInfo OscGetter<ActiveDriveControllerParams, CacheSize>::_osc;

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
bool OscGetter<ActiveDriveControllerParams, CacheSize>::_saveOsc = false;

template<class ActiveDriveControllerParams, unsigned int CacheSize>
OscGetter<ActiveDriveControllerParams, CacheSize>::AppendOscDataCallbackType OscGetter<ActiveDriveControllerParams, CacheSize>::AppendOscDataCallback;

typedef OscGetter<ActiveDriveControllerParams, 13> OscGet;

#endif