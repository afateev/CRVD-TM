#ifndef EMF32_MEMORYSYSTEMCONTROLLER_H
#define EMF32_MEMORYSYSTEMCONTROLLER_H

#include "Register.h"
#include "../IdObjectBase.h"
#include "../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Efm32
{
namespace MemorySystemController
{
	class MemoryMap
	{
	public:
		static const unsigned int BaseAddress = 0x400C0000;
		
		enum DeviceInformationTable
		{
			CMU_LFRCOCTRL 			= 0x0FE08020, // Register reset value
			CMU_HFRCOCTRL 			= 0x0FE08028, // Register reset value
			CMU_AUXHFRCOCTRL 		= 0x0FE08030, // Register reset value
			ADC0_CAL 				= 0x0FE08040, // Register reset value
			ADC0_BIASPROG 			= 0x0FE08048, // Register reset value
			DAC0_CAL 				= 0x0FE08050, // Register reset value
			DAC0_BIASPROG 			= 0x0FE08058, // Register reset value
			ACMP0_CTRL 				= 0x0FE08060, // Register reset value
			ACMP1_CTRL 				= 0x0FE08068, // Register reset value
			CMU_LCDCTRL 			= 0x0FE08078, // Register reset value
			DAC0_OPACTRL 			= 0x0FE080A0, // Register reset value
			DAC0_OPAOFFSET 			= 0x0FE080A8, // Register reset value
			DI_CRC 					= 0x0FE081B0, // [15:0]: DI data CRC-16
			CAL_TEMP_0 				= 0x0FE081B2, // [7:0] Calibration temperature (DegC)
			RESERVED0 				= 0x0FE081B3, // [7:0]: Reserved for other temperature information
			ADC0_CAL_1V25 			= 0x0FE081B4, // [14:8]: Gain for 1V25 reference [6:0]: Offset for 1V25 reference
			ADC0_CAL_2V5 			= 0x0FE081B6, // [14:8]: Gain for 2V5 reference [6:0]: Offset for 2V5 reference
			ADC0_CAL_VDD 			= 0x0FE081B8, // [14:8]: Gain for VDD reference [6:0]: Offset for VDD reference
			ADC0_CAL_5VDIFF 		= 0x0FE081BA, // [14:8]: Gain for 5VDIFF reference [6:0]: Offset for 5VDIFF reference
			ADC0_CAL_2XVDD 			= 0x0FE081BC, // [14:8]: Reserved (gain for this reference cannot be calibrated) [6:0]: Offset for 2XVDD reference
			ADC0_TEMP_0_READ_1V25 	= 0x0FE081BE, // [15:4] Temperature reading at 1V25 reference [3:0] Reserved
			RESERVED1 				= 0x0FE081C0, // [15:0] Reserved for other temperature information
			RESERVED2 				= 0x0FE081C2, // [15:0] Reserved
			RESERVED3 				= 0x0FE081C4, // [31:0] Reserved
			DAC0_CAL_1V25 			= 0x0FE081C8, // [22:16]: Gain for 1V25 reference [13:8]: Channel 1 offset for 1V25 reference [5:0]: Channel 0 offset for 1V25 reference
			DAC0_CAL_2V5 			= 0x0FE081CC, // [22:16]: Gain for 2V5 reference [13:8]: Channel 1 offset for 2V5 reference [5:0]: Channel 0 offset for 2V5 reference
			DAC0_CAL_VDD 			= 0x0FE081D0, // [22:16]: Reserved (gain for this reference cannot be calibrated) [13:8]: Channel 1 offset for VDD reference [5:0]: Channel 0 offset for VDD reference
			AUXHFRCO_CALIB_BAND_1 	= 0x0FE081D4, // [7:0]: 1 MHz tuning
			AUXHFRCO_CALIB_BAND_7 	= 0x0FE081D5, // [7:0]: 7 MHz tuning
			AUXHFRCO_CALIB_BAND_11 	= 0x0FE081D6, // [7:0]: 11 MHz tuning
			AUXHFRCO_CALIB_BAND_14 	= 0x0FE081D7, // [7:0]: 14 MHz tuning
			AUXHFRCO_CALIB_BAND_21 	= 0x0FE081D8, // [7:0]: 21 MHz tuning
			AUXHFRCO_CALIB_BAND_28 	= 0x0FE081D9, // [7:0]: 28 MHz tuning
			HFRCO_CALIB_BAND_1 		= 0x0FE081DC, // [7:0]: 1 MHz tuning
			HFRCO_CALIB_BAND_7 		= 0x0FE081DD, // [7:0]: 7 MHz tuning
			HFRCO_CALIB_BAND_11 	= 0x0FE081DE, // [7:0]: 11 MHz tuning
			HFRCO_CALIB_BAND_14 	= 0x0FE081DF, // [7:0]: 14 MHz tuning
			HFRCO_CALIB_BAND_21 	= 0x0FE081E0, // [7:0]: 21 MHz tuning
			HFRCO_CALIB_BAND_28 	= 0x0FE081E1, // [7:0]: 28 MHz tuning
			RESERVED4 				= 0x0FE081E2, // [15:0] Reserved
			RESERVED5 				= 0x0FE081E4, // 23:0] Reserved
			MEM_INFO_PAGE_SIZE 		= 0x0FE081E7, // [7:0] Flash page size in bytes coded as 2 ^((MEM_INFO_PAGE_SIZE + 10) & 0xFF)
			UNIQUE_0 				= 0x0FE081F0, // [31:0] Unique number
			UNIQUE_1 				= 0x0FE081F4, // [63:32] Unique number
			MEM_INFO_FLASH 			= 0x0FE081F8, // [15:0]: Flash size, kbyte count as unsigned integer (eg. 128)
			MEM_INFO_RAM 			= 0x0FE081FA, // [15:0]: Ram size, kbyte count as unsigned integer (eg. 16)
			PART_NUMBER 			= 0x0FE081FC, // [15:0]: EFM32 part number as unsigned integer (eg. 230)
			PART_FAMILY 			= 0x0FE081FE, // [7:0]: EFM32 part family number (Gecko = 71d, Giant Gecko = 72d, Tiny Gecko = 73d, Leopard Gecko=74d)
			PROD_REV 				= 0x0FE081FF, // [7:0]: EFM32 Production ID
		};
	public:
		static unsigned int GetDeviceInformationTableValue(DeviceInformationTable record)
		{
			unsigned int *ptr = (unsigned int *)record;
			return *ptr;
		}
	};
	
	// MSC_READCTRL - Read Control Register
	class ReadControlRegister
	{
	public:
		static const unsigned int AddressOffset = 0x004;
		static const unsigned int Address = MemoryMap::BaseAddress + AddressOffset;
		
		enum Mode
		{
			ModeWS0				= 0,
			ModeWS1				= 1,
			ModeWS0SCBTP		= 2,
			ModeWS1SCBTP		= 3,
			ModeWS2				= 4,
			ModeWS2SCBTP		= 5,
		};
	protected:
		struct RegStruct
		{
			Core::RegisterValueType ReadMode								: 3;
			Core::RegisterValueType InternalFlashCacheDisable				: 1;
			Core::RegisterValueType AutomaticInvalidateDisable				: 1;
			Core::RegisterValueType InterruptContextCacheDisable			: 1;
			Core::RegisterValueType ExternalBusInterfaceCacheDisable		: 1;
			Core::RegisterValueType RamCacheEnable							: 1;
			Core::RegisterValueType PrefetchMode							: 1;
			Core::RegisterValueType 										: 7;
			Core::RegisterValueType StrategyForBusMatrix					: 2;
			Core::RegisterValueType 										: 14;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void SetReadMode(Mode mode)
		{
			Register::Ptr()->ReadMode = mode;
		}
	};
	
	// MSC_WRITECTRL - Write Control Register
	class WriteControlRegister
	{
	public:
		static const unsigned int AddressOffset = 0x008;
		static const unsigned int Address = MemoryMap::BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType EnableWriteEraseController			: 1;
			Core::RegisterValueType AbortPageEraseOnInterrupt			: 1;
			Core::RegisterValueType WriteTwoWordsAtATime				: 1;
			Core::RegisterValueType LowPowerWrite						: 1;
			Core::RegisterValueType LowPowerErase						: 1;
			Core::RegisterValueType ReadWhileWriteEnable				: 1;
			Core::RegisterValueType 									: 26;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void WriteEnable(bool enable)
		{
			Register::Ptr()->EnableWriteEraseController = enable;
		}
	};
	
	// MSC_WRITECMD - Write Command Register
	class WriteCommandRegister
	{
	public:
		static const unsigned int AddressOffset = 0x00C;
		static const unsigned int Address = MemoryMap::BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType LoadAddressBuffer			: 1;
			Core::RegisterValueType ErasePage					: 1;
			Core::RegisterValueType EndWriteMode				: 1;
			Core::RegisterValueType WordWriteOnceTrigger		: 1;
			Core::RegisterValueType WordWriteSequenceTrigger	: 1;
			Core::RegisterValueType AbortEraseSequence			: 1;
			Core::RegisterValueType 							: 2;
			Core::RegisterValueType MassEraseRegion0			: 1;
			Core::RegisterValueType MassEraseRegion1			: 1;
			Core::RegisterValueType 							: 2;
			Core::RegisterValueType ClearWDATAstate				: 1;
			Core::RegisterValueType							 	: 19;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static void LoadAddress()
		{
			Register::Ptr()->LoadAddressBuffer = 1;
		}
		
		static void ErasePage()
		{
			Register::Ptr()->ErasePage = 1;
		}
		
		static void WriteOnce()
		{
			Register::Ptr()->WordWriteOnceTrigger = 1;
		}
	};
	
	// MSC_ADDRB - Page Erase/Write Address Buffer
	class PageEraseWriteAddressBuffer
	{
	public:
		static const unsigned int AddressOffset = 0x010;
		static const unsigned int Address = MemoryMap::BaseAddress + AddressOffset;
	protected:
		typedef Register<Address, unsigned int> Register;
	public:
		static void SetAddress(unsigned int address)
		{
			*Register::Ptr() = address;
		}
	};
	
	// MSC_WDATA - WriteDataRegister
	class WriteDataRegister
	{
	public:
		static const unsigned int AddressOffset = 0x018;
		static const unsigned int Address = MemoryMap::BaseAddress + AddressOffset;
	protected:
		typedef Register<Address, unsigned int> Register;
	public:
		static void SetData(unsigned int address)
		{
			*Register::Ptr() = address;
		}
	};
	
	// MSC_STATUS - Status Register
	class StatusRegister
	{
	public:
		static const unsigned int AddressOffset = 0x01C;
		static const unsigned int Address = MemoryMap::BaseAddress + AddressOffset;
	protected:
		struct RegStruct
		{
			Core::RegisterValueType EraseWriteBusy						: 1;
			Core::RegisterValueType AccessLocked						: 1;
			Core::RegisterValueType InvalidWriteAddressOrErasePage		: 1;
			Core::RegisterValueType WDATAWriteReady						: 1;
			Core::RegisterValueType FlashWriteWordTimeout				: 1;
			Core::RegisterValueType CurrentFlashEraseOperationAborted	: 1;
			Core::RegisterValueType PerformanceCountersRunning			: 1;
			Core::RegisterValueType 									: 25;
		};
		
		typedef Register<Address, RegStruct> Register;
	public:
		static bool Busy()
		{
			return Register::Ptr()->EraseWriteBusy;
		}
		
		static bool DataWriteReady()
		{
			return Register::Ptr()->WDATAWriteReady;
		}
	};
	
	class MemorySystemController :
		public ReadControlRegister,
		public WriteControlRegister,
		public WriteCommandRegister,
		public PageEraseWriteAddressBuffer,
		public WriteDataRegister,
		public StatusRegister
	{
	public:
		static unsigned int FlashSize()
		{
			return MemoryMap::GetDeviceInformationTableValue(MemoryMap::MEM_INFO_FLASH) * 1024;
		}
		
		static unsigned int FlashPageSize()
		{
			int power = MemoryMap::GetDeviceInformationTableValue(MemoryMap::MEM_INFO_PAGE_SIZE) + 10 & 0xFF;
			int size = 1;
			for (int i = 0; i < power; i++)
			{
				size *= 2;
			}
			return size;
		}
		
		static unsigned char Get_HFRCO_CALIB_BAND_28()
		{
			return MemoryMap::GetDeviceInformationTableValue(MemoryMap::HFRCO_CALIB_BAND_28);
		}
		
		// Получить стартовый адрес сегмента и его размер, по переданному адресу
		static void GetSegmentBounds(unsigned int address, unsigned int &segmentStart, unsigned int &segmentSize)
		{
			unsigned int start = 0;
			unsigned int size = 0;

			// Main memory
			if (address <= FlashSize())
			{
				start = 0;		// начало области памяти
				size = FlashPageSize();	// размер сегментов
			}

			// если не попали в допустимую область
			if (size == 0)
			{
				// возвращаем нулевой размер
				segmentSize = size;
				return;
			}

			// вычисляем начало сегмента
			
			// вычисляем смещение от начала блока
			segmentStart = address - start;
			// сколько сегментов состаляет это смещение
			segmentStart /= size;
			// сколько целых сегментов
			segmentStart *= size;
			// добавляя начало к размеру целых сегментов получаем адрес начала сегмента
			segmentStart += start;
			// размер сегмента
			segmentSize = size;
		}
		
		static void Erase(unsigned int address)
		{
			while(Busy())
			{
				;
			}
			
			WriteEnable(true);
			SetAddress(address);
			LoadAddress();
			ErasePage();
			while(Busy())
			{
				;
			}
			WriteEnable(false);
		}
		
		static void WriteInt32(unsigned int address, unsigned int value)
		{
			while(Busy())
			{
				;
			}
			
			WriteEnable(true);
			SetAddress(address);
			LoadAddress();
			while(!DataWriteReady())
			{
				;
			}
			SetData(value);
			WriteOnce();
			while(Busy())
			{
				;
			}
			WriteEnable(false);
		}
		
		template
			<
				class TmpSpace
			>
		static bool Write(unsigned int address, unsigned char *data, unsigned int len)
		{
			// позиция в источнике данных
			unsigned int srcPos = 0;
			// позиции начала и конца области записи флэшки
			unsigned int writePos = address;
			unsigned int writeEnd = address + len - 1;
			// начала и размер сегмента где находится начало блока
			unsigned int firstSegmentStart;
			unsigned int firstSegmentSize;
			// начала и размер сегмента где находится конец блока
			unsigned int lastSegmentStart;
			unsigned int lastSegmentSize;
			// начало текущего сегмента
			unsigned int currentSegmentStart = 0;
			// в каком сегменте будем начинать запись
			GetSegmentBounds(writePos, firstSegmentStart, firstSegmentSize);
			// в каком сегменте закончим запись
			GetSegmentBounds(writeEnd, lastSegmentStart, lastSegmentSize);
			// если сюда нельзя записывать, выходим
			if (firstSegmentSize == 0 || lastSegmentSize == 0)
			{
				return false;
			}
			
			do
			{
				// начало и размер сегмента куда сейчас пишем
				unsigned int segmentStart;
				unsigned int segmentSize;
				GetSegmentBounds(writePos, segmentStart, segmentSize);
				// если начался новый сегмент
				if (currentSegmentStart != segmentStart)
				{
					currentSegmentStart = segmentStart;
					writePos = currentSegmentStart;
					// копируем сегмент
					TmpSpace::Write(0, (unsigned char *)segmentStart, segmentSize);
					
					// стираем сегмент
					while(Busy())
					{
						;
					}
					
					WriteEnable(true);
					SetAddress(address);
					LoadAddress();
					ErasePage();
					// подождем пока сотрётся
					while(Busy())
					{
						;
					}
				}
				if (writePos < address)
				{
					while(!DataWriteReady())
					{
						;
					}
					
					unsigned int value = 0;
					
					int delta = address - writePos;
					if (delta < 4)
					{
						value = TmpSpace::ReadInt32(writePos - currentSegmentStart);
						unsigned char *valueAsArray = (unsigned char *)&value;
						for (int i = delta; i < 4; i++)
						{
							if (srcPos >= len)
							{
								break;
							}
							
							valueAsArray[i] = data[srcPos];
							srcPos++;
						}
					}
					else
					{
						value = TmpSpace::ReadInt32(writePos - currentSegmentStart);
					}
					
					SetData(value);
					WriteOnce();
				}
				else
				{
					while(!DataWriteReady())
					{
						;
					}
					
					unsigned int value = 0;
					
					int delta = writeEnd + 1 - writePos;
					if (delta < 4)
					{
						value = TmpSpace::ReadInt32(writePos - currentSegmentStart);
						unsigned char *valueAsArray = (unsigned char *)&value;
						for (int i = 0; i < delta; i++)
						{
							valueAsArray[i] = data[srcPos];
							srcPos++;
						}
					}
					else
					{
						value = *((unsigned int *)&data[srcPos]);
						srcPos += 4;
					}
					
					SetData(value);
					WriteOnce();
				}

				writePos += 4;
			}
			while(writePos <= writeEnd);

			// дописываем оставшуюся часть последнего сегмента
			while(writePos < lastSegmentStart + lastSegmentSize)
			{
				while(!DataWriteReady())
				{
					;
				}
				SetData(TmpSpace::ReadInt32(writePos - lastSegmentStart));
				WriteOnce();
				writePos += 4;
			}
			
			WriteEnable(false);
			return true;
		}
	};
}
}
}

#endif