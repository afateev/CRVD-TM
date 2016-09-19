////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		01/09/2016
////////////////////////////////////////////////////////////////////////////////

#ifndef WINDOW_DEBUG_OSC_H
#define WINDOW_DEBUG_OSC_H

#include "Window.h"

template<class DisplayType, DisplayType &display>
class WindowDebugOsc : public Window<DisplayType, display>
{
public:
	struct DisplayData
	{
		static const int EventCount = 5;
		
		struct EventDataStruct
		{
			unsigned int File;
			int Pointer;
		};
	public:
		unsigned int CacheFileNumber;
		unsigned int CurrentOscPos;
		unsigned int LoadedOscPos;
		EventDataStruct EventData[EventCount];
	public:
		DisplayData()
		{
			CacheFileNumber = 0;
			CurrentOscPos = 0;
			LoadedOscPos = 0;
			
			for (int i = 0; i < EventCount; i++)
			{
				EventData[i].File = 0;
				EventData[i].Pointer = -1;
			}
		}
	};
	
	typedef Rblib::CallbackWrapper<DisplayData &> GetDisplayDataCallbackType;
public:
	GetDisplayDataCallbackType GetDisplayDataCallback;
public:
	virtual void Draw()
	{
		DisplayData displayData;
		GetDisplayDataCallback(displayData);
		
		char str[100];
		unsigned char len;
		
		len = sprintf(str, "CacheFileNumber: %d", displayData.CacheFileNumber);
		display.MoveCursorTo(0, 16 * 1);
		display.WriteLine(str, len);
		
		len = sprintf(str, "CurrentOscPos: %d", displayData.CurrentOscPos);
		display.MoveCursorTo(0, 16 * 2);
		display.WriteLine(str, len);
		
		int delta = displayData.CurrentOscPos - displayData.LoadedOscPos;
		if (delta < 0)
		{
			delta += 65535;
		}
		
		len = sprintf(str, "RequestOscPos: %d (delta: %d)", displayData.LoadedOscPos, delta);
		display.MoveCursorTo(0, 16 * 3);
		display.WriteLine(str, len);
		
		for (int i = 0; i < DisplayData::EventCount; i++)
		{
			len = sprintf(str, "Event %d: file %d, pointer: %d", i + 1, displayData.EventData[i].File, displayData.EventData[i].Pointer);
			display.MoveCursorTo(0, 16 * 4 + i * 16);
			display.WriteLine(str, len);
		}
	}
	
	virtual bool OnKeyDown(char &key)
	{
		if ('E' == key)
		{
			WindowBase::Close();
			return true;
		}
		return false;
	}
};

#endif