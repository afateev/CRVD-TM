////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		21/01/2013
////////////////////////////////////////////////////////////////////////////////

#ifndef OSCFILEFORMAT_H
#define OSCFILEFORMAT_H

class OscFileFormat
{
public:
	enum OscType
	{
		OscTypeEngineStart = 0,
		OscTypeEngineStop,
		OscTypeEngineEvent,
		OscTypeCheckoutStart,
		OscTypeCheckoutStop
	};
	
	static const unsigned short FramesPerSec = 200;
	static const unsigned char FileNameSize = 35;
	struct OscFileDescription
	{
		char FileName[FileNameSize];
	};
	
	struct HeaderStruct
	{
		unsigned short Version;
		unsigned short Trash;
		unsigned int Length;
		unsigned int Center;
		
		HeaderStruct()
		{
			Version = 0x0200;
			Trash = 0;
			Length = 0;
			Center = 0;
		}
	};
	
	struct AnalogValuesStruct
	{
		float Ust;
		float Ist;
		float Urot;
		float Irot;
		
		AnalogValuesStruct()
		{
			Ust = 0.0;
			Ist = 0.0;
			Urot = 0.0;
			Irot = 0.0;
		}
	};
	
	struct DataStruct
	{
		unsigned short Ust;
		unsigned short Ist;
		unsigned short Urot;
		unsigned short Irot;
		unsigned int Discrete;
		
		void SwapBytes()
		{
			SwapBytes(&Ust);
			SwapBytes(&Ist);
			SwapBytes(&Urot);
			SwapBytes(&Irot);
            SwapBytes(&Discrete);
		}
	protected:
		void SwapBytes(unsigned short * word)
		{
			if (!word)
			{
				return;
			}
			unsigned short tmp = *word;
			*word >>= 8;
			tmp <<= 8;
			*word |= tmp;
		}
        
        void SwapBytes(unsigned int * dword)
		{
			if (!dword)
			{
				return;
			}
            
            unsigned short wordlo = *dword & 0xFFFF;
            unsigned short wordhi = (*dword >> 16) & 0xFFFF;
            SwapBytes(&wordlo);
            SwapBytes(&wordhi);
            
            *dword = wordhi;
            *dword <<= 16;
            *dword |= wordlo;
		}
	};
	
	static void FormatFileName(char *buffer, unsigned int bufferSize, time_t oscTime, OscType oscType)
	{
		struct tm * timeinfo;
		timeinfo = localtime(&oscTime);
		size_t len = strftime(buffer, bufferSize, "/osc/%Y%m%d_%H%M%S", timeinfo);
		
		if (len > 0)
		{
			switch(oscType)
			{
			case OscTypeEngineStart:
					strcpy(&buffer[len], "_EngineStart.osc");
				break;
			case OscTypeEngineStop:
					strcpy(&buffer[len], "_EngineStop.osc");
				break;
			case OscTypeEngineEvent:
					strcpy(&buffer[len], "_EngineEvent.osc");
				break;
			case OscTypeCheckoutStart:
					strcpy(&buffer[len], "_CheckoutStart.osc");
				break;
			case OscTypeCheckoutStop:
					strcpy(&buffer[len], "_CheckoutStop.osc");
				break;
			}
		}
	}
};

#endif