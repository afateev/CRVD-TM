///////////////////////////////////////////////////////////////////////////////
//	������ ���������� ������� ������� ������� ���
//
//	������ ������ ������ ��������� �� ������ ������������� ��� �� ����������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef ADC_PARTS_H
#define ADC_PARTS_H

namespace Rblib
{
// ����� ��� ����������� ��� ��������� MSP430
namespace Msp430
{
	// ��������� �������� ���������� 0
	struct AdcControl0Register
	{
		unsigned short StartConversion				: 1; //
		unsigned short EnableConversion				: 1; //
		unsigned short ConversionTimeOverflowInterruptEnable	: 1; //
		unsigned short OverflowInterruptEnable		: 1; //
		unsigned short AdcEnable					: 1; //
		unsigned short ReferenceGeneratorEnable		: 1; //
		unsigned short ReferenceGeneratorVoltage	: 1; //
		unsigned short MultipleConversion			: 1; //
		unsigned short SampleAndHoldTime0			: 4; //
		unsigned short SampleAndHoldTime1			: 4; //
	};

	// ��������� �������� ���������� 1
	struct AdcControl1Register
	{
		unsigned short Busy							: 1; //
		unsigned short ConversionSequenceMode		: 2; //
		unsigned short ClockSource					: 2; //
		unsigned short ClockDivider					: 3; //
		unsigned short InvertSamleAndHolsSignal		: 1; //
		unsigned short SampleAndHoldPulseMode		: 1; //
		unsigned short SampleAndHoldSource			: 2; //
		unsigned short ConversionStartAddress		: 4; //
	};

	// ��������� �������� ���������� ������� ��������������
	struct AdcMemoryControlRegister
	{
		unsigned char InputChannel				: 4; // �����
		unsigned char Reference					: 3; // �������� �������� ����������
		unsigned char EndOfSequence				: 1; // ������� ����� ������������������
	};

	// ����� ��������� ���������� ���
	template <class RegisterDataType,			// ��� (������) �������� ��������
			class PointerType,					// ��� (������) �������� ������� ���������
			PointerType Control0RegisterAddr,	// ����� �������� 0 ����������
			PointerType Control1RegisterAddr,	// ����� �������� 1 ����������
			class Control0RegisterStructure,	// ��������� �������� 0 ����������
			class Control1RegisterStructure		// ��������� �������� 1 ����������
		>
	class AdcControl
	{
	protected:
		// ������ ��������� ����������
		static volatile Control0RegisterStructure * const _adcControl0RegisterAddr;
		static volatile Control1RegisterStructure * const _adcControl1RegisterAddr;
		
	public:
		// �������� ������ ���
		static inline void Enable()
		{
		  	_adcControl0RegisterAddr->AdcEnable = 1;
			_adcControl1RegisterAddr->SampleAndHoldPulseMode = 1;
		}
		// ��������� ������ ���
		static inline void Disable()
		{
			_adcControl0RegisterAddr->AdcEnable = 0;
		}
		static inline bool IsEnabled()
		{
			return _adcControl0RegisterAddr->AdcEnable == 1;
		}
		// ��������� ������ ������ ������
		static inline void ConvertOneSample(unsigned short channel)
		{
			// ���� ������ ������ �� �������, �� ������ ������
			// ���� �� ���������, �� ����� ��������� � �������� ����� Busy
			if (!IsEnabled())
				return;
			// ��� ����� ������ � ������ ��������
			_adcControl1RegisterAddr->ConversionStartAddress = channel;
			// �������� ��������������
			_adcControl0RegisterAddr->EnableConversion = 1;
			// �������� ������
			_adcControl0RegisterAddr->StartConversion = 1;
			// ��� ���� ���������� �������
			while (_adcControl1RegisterAddr->Busy);
			// ��������� ��������������
			_adcControl0RegisterAddr->EnableConversion = 0;
		}
		// ������ �������� ������������
		static inline void SetClockSource(unsigned short value)
		{
			_adcControl1RegisterAddr->ClockSource = value;
		}
		// ���������� �������� ��������� �������
		static inline void SetDivider(unsigned short value)
		{
			_adcControl1RegisterAddr->ClockDivider = value;
		}
		// ��������� ������������ ��������� ��� ������� 0-7
		static inline void SetSampleAndHoldTimeFrom0To7(unsigned char value)
		{
			_adcControl0RegisterAddr->SampleAndHoldTime0 = value;
		}
		// ��������� ������������ ��������� ��� ������� 8-15
		static inline void SetSampleAndHoldTimeFrom8To15(unsigned char value)
		{
			_adcControl0RegisterAddr->SampleAndHoldTime1 = value;
		}
		// ��������� ������������ ��������� ��� ���� �������
		static inline void SetSampleAndHoldTime(unsigned char value)
		{
			SetSampleAndHoldTimeFrom0To7(value);
			SetSampleAndHoldTimeFrom8To15(value);
		}
	};

	// ��������������� ����������� ����������
	template<class RegisterDataType, class PointerType, PointerType Control0RegisterAddr, PointerType Control1RegisterAddr, class Control0RegisterStructure, class Control1RegisterStructure>
	volatile  Control0RegisterStructure* const AdcControl<RegisterDataType, PointerType, Control0RegisterAddr, Control1RegisterAddr, Control0RegisterStructure, Control1RegisterStructure>
		::_adcControl0RegisterAddr = reinterpret_cast<volatile  Control0RegisterStructure*>(Control0RegisterAddr);

	template<class RegisterDataType, class PointerType, PointerType Control0RegisterAddr, PointerType Control1RegisterAddr, class Control0RegisterStructure, class Control1RegisterStructure>
	volatile  Control1RegisterStructure* const AdcControl<RegisterDataType, PointerType, Control0RegisterAddr, Control1RegisterAddr, Control0RegisterStructure, Control1RegisterStructure>
		::_adcControl1RegisterAddr = reinterpret_cast<volatile  Control1RegisterStructure*>(Control1RegisterAddr);

	// ����� �������� ���������� ������� ���������� � ����������� ��������������
	template <class RegisterDataType,				// ��� (������) �������� ��������
			class PointerType,						// ��� (������) �������� ������� ���������
			PointerType MemoryControlRegisterAddr,	// ����� �������� ����������
			PointerType MemoryRegisterAddr,			// ����� �������� ����������
			class MemoryControlRegisterStructure	// ��������� �������� ����������
		>
	class AdcMemory
	{
	protected:
		// ������ ��������� ����������
		static volatile MemoryControlRegisterStructure * const _memoryControlRegisterAddr;
		static volatile RegisterDataType * const _memoryRegisterAddr;
	public:
		// ���������� ����� ������
		static inline void SetChannel(unsigned char value)
		{
			_memoryControlRegisterAddr->InputChannel = value;
		}
		// ���������� �������� �������� ����������
		static inline void SetRefSource(unsigned char value)
		{
			_memoryControlRegisterAddr->Reference = value;
		}
		// �������� ��������� ��������������
		static inline RegisterDataType GetResult()
		{
			return *_memoryRegisterAddr;
		}
	};
	
	// ��������������� ����������� ����������
	template<class RegisterDataType, class PointerType, PointerType MemoryControlRegisterAddr, PointerType MemoryRegisterAddr, class MemoryControlRegisterStructure>
	volatile  MemoryControlRegisterStructure* const AdcMemory<RegisterDataType, PointerType, MemoryControlRegisterAddr, MemoryRegisterAddr, MemoryControlRegisterStructure>
		::_memoryControlRegisterAddr = reinterpret_cast<volatile  MemoryControlRegisterStructure*>(MemoryControlRegisterAddr);

	template<class RegisterDataType, class PointerType, PointerType MemoryControlRegisterAddr, PointerType MemoryRegisterAddr, class MemoryControlRegisterStructure>
	volatile  RegisterDataType* const AdcMemory<RegisterDataType, PointerType, MemoryControlRegisterAddr, MemoryRegisterAddr, MemoryControlRegisterStructure>
		::_memoryRegisterAddr = reinterpret_cast<volatile RegisterDataType*>(MemoryRegisterAddr);
	
}
}

#endif