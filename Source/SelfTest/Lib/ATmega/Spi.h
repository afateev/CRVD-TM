///////////////////////////////////////////////////////////////////////////////
//	 ласс управлени€ портами SPI
//	ATMEGA
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef SPI_ATMEGA_H
#define SPI_ATMEGA_H

#include "../IdObjectBase.h"
#include "../RegisterReference.h"

namespace Lib
{
namespace ATmega
{
namespace Spi
{
	// “ип значени€ порта
	typedef unsigned char ValueDataType;
	
	// SPCR Ц SPI Control Register
	template
		<
			RegisterPointerType RegisterAddr
		>
	class ControlRegister
	{
	public:
		struct RegisterStruct
		{
			ValueDataType ClockRateSelect 	: 2;
			ValueDataType ClockPhase 			: 1;
			ValueDataType ClockPolarity 		: 1;
			ValueDataType MasterSlaveSelect 	: 1;
			ValueDataType DataOrder 			: 1;
			ValueDataType Enable 				: 1;
			ValueDataType InterruptEnable 		: 1;
		};
		
		enum ClockDiv
		{
			ClockDiv4	= 0,
			ClockDiv16	= 1,
			ClockDiv64	= 2,
			ClockDiv128	= 3,
		};
	protected:
		typedef RegisterReference<RegisterStruct, RegisterAddr> RegRef;
	public:
		static void SetClockRate(ClockDiv clockDiv)
		{
			RegRef::_register->ClockRateSelect = clockDiv;
		}
		
		static void SetClockPhase(bool val)
		{
			RegRef::_register->ClockPhase = val;
		}
		
		static void SetClockPolarity(bool val)
		{
			RegRef::_register->ClockPolarity = val;
		}
		
		static void SetMasterMode(bool master)
		{
			RegRef::_register->MasterSlaveSelect = master;
		}
		
		static void SetDataOrder(bool lsbFirst)
		{
			RegRef::_register->DataOrder = lsbFirst;
		}
		
		static void Enable(bool enable)
		{
			RegRef::_register->Enable = enable;
		}
		
		static void InterruptEnable(bool enable)
		{
			RegRef::_register->InterruptEnable = enable;
		}
	};
	
	// SSPSR Ц SPI Status Register
	template
		<
			RegisterPointerType RegisterAddr
		>
	class StatusRegister
	{
	public:
		struct RegisterStruct
		{
			ValueDataType DoubleSpeed 			: 1;
			ValueDataType  						: 5;
			ValueDataType WriteCollisionFlag 	: 1;
			ValueDataType InterruptFlag	 		: 1;
		};
	protected:
		typedef RegisterReference<RegisterStruct, RegisterAddr> RegRef;
	public:
		static void SetDoubleSpeed(bool enable)
		{
			RegRef::_register->DoubleSpeed = enable;
		}
		
		static bool IsWriteCollisionFlag()
		{
			return RegRef::_register->WriteCollisionFlag;
		}
		
		static bool IsInterruptFlag()
		{
			return RegRef::_register->InterruptFlag;
		}
	};
	
	// SPDR Ц SPI Data Register
	template
		<
			RegisterPointerType RegisterAddr
		>
	class DataRegister
	{
	protected:
		typedef RegisterReference<unsigned char, RegisterAddr> RegRef;
	public:
		static void WriteData(unsigned char val)
		{
			*RegRef::_register = val;
		}
		
		static unsigned char ReadData()
		{
			return *RegRef::_register;
		}
	};
	
	template
		<
			unsigned int IdObj,
			RegisterPointerType ControlRegisterAddr,
			RegisterPointerType StatusRegisterAddr,
			RegisterPointerType DataRegisterAddr
		>
	class Spi :
		public ControlRegister<ControlRegisterAddr>,
		public StatusRegister<StatusRegisterAddr>,
		public DataRegister<DataRegisterAddr>
	{
	public:
		typedef ControlRegister<ControlRegisterAddr> ControlRegister;
		typedef StatusRegister<StatusRegisterAddr> StatusRegister;
		typedef DataRegister<DataRegisterAddr> DataRegister;
	public:
		static void Init()
		{
			ControlRegister::SetMasterMode(true);
			ControlRegister::SetClockRate(ControlRegister::ClockDiv4);
			
			ControlRegister::SetDataOrder(false);
			ControlRegister::SetClockPhase(false);
			ControlRegister::SetClockPolarity(false);
			
			ControlRegister::Enable(true);
		}
		
		static unsigned char Write(unsigned char b)
		{
			DataRegister::WriteData(b);
			while(!StatusRegister::IsInterruptFlag())
			{
				;
			}
			return DataRegister::ReadData();
		}
	};
}
}
}

#endif