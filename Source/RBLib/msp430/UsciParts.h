///////////////////////////////////////////////////////////////////////////////
//	������ ���������� ������� ������� ������� USCI
//
//	������ ������ ������ ��������� �� ������ ������������� ��� �� ����������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef USCI_PARTS_MSP430_H
#define USCI_PARTS_MSP430_H

namespace Rblib
{
// ����� USCI ����������� ��� ��������� MSP430 ��� ���� ��������
namespace Msp430
{
	//////////////////////////////////////////////////////////////
	// ������� ���������� 0
	//////////////////////////////////////////////////////////////
	
	// ��������� �������� ���������� 0
	// � ������ Uart
	template<class RegisterDataType>
	struct UsciControlRegister0Uart
	{
		RegisterDataType SynchronousModeEnable	: 1;	// ���������� �����
		RegisterDataType Mode					: 2;	// ����� ������ (������� Uart, ������������������, � �������� �����, � �������������� ������� ����������� ��������)
		RegisterDataType StopBits				: 1;	// ���������� �������� �����
		RegisterDataType CharacterLength		: 1;	// ����� �������� (7 ��� 8 ���)
		RegisterDataType MsbFirstEnable			: 1;	// ������� ���
		RegisterDataType Parity					: 2;	// ��������
	};
	
	// ��������� �������� ���������� 0
	// � ������ SPI
	template<class RegisterDataType>
	struct UsciControlRegister0Spi
	{
		RegisterDataType SynchronousModeEnable	: 1;	// ���������� �����
		RegisterDataType Mode					: 2;	// ����� ������ (3-pin SPI, 4-pin SPI, I2C)
		RegisterDataType Master					: 1;	// Master / Slave
		RegisterDataType CharacterLength		: 1;	// ����� �������� (7 ��� 8 ���)
		RegisterDataType MsbFirstEnable			: 1;	// ������� ���
		RegisterDataType ClockPolarity			: 1;	// ���������� �������� ���������
		RegisterDataType ClockPhase				: 1;	// ���� �������� ���������
	};
	
	// ��������� �������� ���������� 0
	// � ������ I2C
	template<class RegisterDataType>
	struct UsciControlRegister0I2C
	{
		RegisterDataType SynchronousModeEnable	: 1;	// ���������� �����
		RegisterDataType Mode					: 2;	// ����� ������ (3-pin SPI, 4-pin SPI, I2C)
		RegisterDataType Master					: 1;	// Master / Slave
		RegisterDataType Reserved				: 1;	// 
		RegisterDataType MultiMasterEnable		: 1;	// ����� � ����������� ���������
		RegisterDataType SlaveAddressSize		: 1;	// ������ ������ �������� (7 ��� 10 ���)
		RegisterDataType OwnAddressSize			: 1;	// ������ ������ ������ (7 ��� 10 ���)
	};

	//////////////////////////////////////////////////////////////
	// ������� ���������� 1
	//////////////////////////////////////////////////////////////

	// ��������� �������� ���������� 1
	// � ������ Uart
	template<class RegisterDataType>
	struct UsciControlRegister1Uart
	{
		RegisterDataType Reset									: 1;	// �����
		RegisterDataType TransmitBreak							: 1;	// �������� Break � ��������� �����
		RegisterDataType TransmitAddress						: 1;	// �������� ����� � ��������� �����
		RegisterDataType Dormant								: 1;	// ������� � �������� Uart
		RegisterDataType ReceiveBreakInterruptEnable			: 1;	// ��������� ���������� Rx ��� �������� Break
		RegisterDataType ReceiveErroneousCharInterruptEnable	: 1;	// ��������� ���������� ��� ��������� ����� ��������
		RegisterDataType ClockSource							: 2;	// �������� ������������
	};

	// ��������� �������� ���������� 1
	// � ������ SPI
	template<class RegisterDataType>
	struct UsciControlRegister1Spi
	{
		RegisterDataType Reset			: 1;	// �����
		RegisterDataType Reserved		: 5;	// 
		RegisterDataType ClockSource	: 2;	// �������� ������������
	};

	// ��������� �������� ���������� 1
	// � ������ I2C
	template<class RegisterDataType>
	struct UsciControlRegister1I2C
	{
		RegisterDataType Reset				: 1;	// �����
		RegisterDataType TransmitStart		: 1;	// Generate START condition
		RegisterDataType TransmitStop		: 1;	// Generate STOP
		RegisterDataType TransmitNack		: 1;	// Generate NACK
		RegisterDataType TransmitterEnable	: 1;	// 1 - ����������, 0 - ��������
		RegisterDataType Reserved			: 1;	// 
		RegisterDataType ClockSource		: 2;	// �������� ������������
	};
	
	//////////////////////////////////////////////////////////////
	// ������� ���������� ����������
	//////////////////////////////////////////////////////////////

	// ��������� �������� ���������� ����������
	template<class RegisterDataType>
	struct UsciModulationControlRegister
	{
		RegisterDataType OversamplingEnable		: 1;	// 
		RegisterDataType SecondModulationStage	: 3;	// 
		RegisterDataType FirstModulationStage	: 4;	// 
	};

	//////////////////////////////////////////////////////////////
	// ������� �������
	//////////////////////////////////////////////////////////////

	// ��������� �������� �������
	// � ������ Uart
	template<class RegisterDataType>
	struct UsciStatusRegisterUart
	{
		RegisterDataType Busy					: 1;	// 
		RegisterDataType IdleOrAddressReceived	: 1;	// 
		RegisterDataType RxError				: 1;	// 
		RegisterDataType Break					: 1;	// 
		RegisterDataType ParityError			: 1;	// 
		RegisterDataType OverrunError			: 1;	// 
		RegisterDataType FramingError			: 1;	// 
		RegisterDataType Listen					: 1;	// 
	};

	// ��������� �������� �������
	// � ������ Spi
	template<class RegisterDataType>
	struct UsciStatusRegisterSpi
	{
		RegisterDataType Busy			: 1;	// 
		RegisterDataType Reserved		: 4;	// 
		RegisterDataType OverrunError	: 1;	// 
		RegisterDataType FramingError	: 1;	// 
		RegisterDataType Listen			: 1;	// 
	};

	//////////////////////////////////////////////////////////////
	// ���������� ������� GPIO
	//////////////////////////////////////////////////////////////

	// ����� ���������� ������� Gpio, ���������� � �����������
	template<class port1, unsigned char pin1, class port2, unsigned char pin2, class port3 = int, unsigned char pin3 = 0>
	class GpioPeripheralFunctionControl
	{
	public:
		typedef port1 Port1;
		typedef port2 Port2;
		typedef port3 Port3;
		static const unsigned char Pin1 = pin1;
		static const unsigned char Pin2 = pin2;
		static const unsigned char Pin3 = pin3;
	public:
		static inline void EnableUartFunction()
		{
			Port1::SetFunctionPeripheral(Pin1);
			Port2::SetFunctionPeripheral(Pin2);
		}

		static inline void EnableI2CFunction()
		{
			Port1::SetFunctionPeripheral(Pin1);
			Port2::SetFunctionPeripheral(Pin2);
		}
		
		static inline void EnableSpiFunction()
		{
			Port1::SetFunctionPeripheral(Pin1);
			Port2::SetFunctionPeripheral(Pin2);
			Port3::SetFunctionPeripheral(Pin3);
		}
		
		static inline void DisableUartFunction()
		{
			Port1::SetFunctionGeneralIo(Pin1);
			Port2::SetFunctionGeneralIo(Pin2);
		}

		static inline void DisabelI2CFunction()
		{
			Port1::SetFunctionGeneralIo(Pin1);
			Port2::SetFunctionGeneralIo(Pin2);
		}
		
		static inline void DisabelSpiFunction()
		{
			Port1::SetFunctionGeneralIo(Pin1);
			Port2::SetFunctionGeneralIo(Pin2);
			Port3::SetFunctionGeneralIo(Pin3);
		}
	};
}
}

#endif