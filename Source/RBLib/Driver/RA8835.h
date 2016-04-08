////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		30/11/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef RBLIB_DRIVER_RA8835_H
#define RBLIB_DRIVER_RA8835_H

namespace Rblib
{
namespace Driver
{
	template<class DataBus>
	class RA8835
	{
	public:
		enum Commands
		{
			CmdSystemSet				= 0x40,	// Initialize device and display
			CmdWrite					= 0x42,	// Write to display memory
			CmdRead						= 0x43,	// Read from display memory
			CmdScroll					= 0x44,	// Set display start address and display regions
			CmdSetCursorAddr			= 0x46,	// Set cursor address
			CmdReadCursorAddr			= 0x47,	// Read cursor address
			CmdSetCursorDirRight		= 0x4C,	// Set direction of cursor movement
			CmdSetCursorDirLeft			= 0x4D,	// Set direction of cursor movement
			CmdSetCursorDirUp			= 0x4E,	// Set direction of cursor movement
			CmdSetCursorDirDown			= 0x4F,	// Set direction of cursor movement
			CmdSleepIn					= 0x53,	// Enter standby mode
			CmdDisplayOff				= 0x58,	// Enable and disable display and display flashing
			CmdDisplayOn				= 0x59,	// Enable and disable display and display flashing
			CmdSetHorScroll				= 0x5A,	// Set horizontal scroll position 
			CmdOverlay					= 0x5B,	// Set display overlay format
			CmdSetCharGeneratorAddr		= 0x5C,	// Set start address of character generator RAM
			CmdSetCursorType			= 0x5D	// Set cursor type
		};
		
		unsigned int Width;
		unsigned int Height;
	public:
		void Init(bool secondBuffer)
		{
			Width = 40;
			Height  = 240;
			
			DataBus::Init();
		
			DataBus::WriteCommand(CmdSystemSet);
			//								7	6	5	4	3	2	1	0
			DataBus::WriteData(0x30);	//	0	0	IV	1	W/S	M2	M1	M0
			DataBus::WriteData(0x87);	//	WF	0	0	0	0	|---FX---|
			DataBus::WriteData(0x07);	//	0	0	0	0	|-----FY-----|
			DataBus::WriteData(39);		//	|------------C/R-------------|	C/R+1 = ширина экрана
			DataBus::WriteData(43);		//	|-----------TC/R-------------|	должно быть C/R+4 или больше время гашения строки
			DataBus::WriteData(239);	//	|------------L/F-------------|	L/F+1 = число строк в кадре
			DataBus::WriteData(40);		//	|------------APL-------------|	AP - address per line
			DataBus::WriteData(0);		//	|------------APH-------------|
			DataBus::WriteStop();
			
			if (secondBuffer)
			{
				DataBus::WriteCommand(CmdScroll);
				DataBus::WriteData(0x00);	//	SAD1L
				DataBus::WriteData(0x00);	//	SAD1H
				DataBus::WriteData(239);	//	SL1
				DataBus::WriteData(0x80);	//	SAD2L
				DataBus::WriteData(0x25);	//	SAD2H
				DataBus::WriteData(239);	//	SL2
				//DataBus::WriteData(0x00);	//	SAD3L
				//DataBus::WriteData(0x00);	//	SAD3H
				//DataBus::WriteData(0x00);	//	SAD4L
				//DataBus::WriteData(0x00);	//	SAD5H
				DataBus::WriteStop();
				
				//								7	6	5	4	3	2	1	0
				DataBus::WriteCommand(CmdOverlay);
				DataBus::WriteData(0x0F);	//	0	0	0	OV	DM2	DM1	MX1	MX0
				DataBus::WriteStop();
				
				//								7	6	5	4	3	2	1	0
				DataBus::WriteCommand(0x59);
				DataBus::WriteData(0x34);	//	FP5	FP4	FP3	FP2	FP1	FP0	FC1	FC0
				DataBus::WriteStop();
			}
			else
			{
				DataBus::WriteCommand(0x59);
				DataBus::WriteData(0x14);	//	FP5	FP4	FP3	FP2	FP1	FP0	FC1	FC0
				DataBus::WriteStop();
			}
		}
		
		void Clear()
		{
			SetCursorAddress(0);
			
			DataBus::WriteCommand(CmdWrite);
			for (int y = 0; y < Height; y++)
			for (int x = 0; x < Width; x++)
			{
				DataBus::WriteData(0x00);
			}
			DataBus::WriteStop();
			
			SetCursorAddress(0);
		}
		
		void WriteByte(unsigned char value)
		{
			DataBus::WriteCommand(CmdWrite);
			DataBus::WriteData(value);
			DataBus::WriteStop();
		}
		
		void WriteBuffer(unsigned char *buf, unsigned int len)
		{
			DataBus::WriteCommand(CmdWrite);
			for (unsigned int i = 0; i < len; i++)
				DataBus::WriteData(buf[i]);
			DataBus::WriteStop();
		}
		
		void SetCursorAddress(unsigned short addr)
		{
			DataBus::WriteCommand(CmdSetCursorAddr);
			DataBus::WriteData(addr & 0xFF);
			DataBus::WriteData((addr >> 8) & 0xFF);
			DataBus::WriteStop();
		}
	};
}
}

#endif