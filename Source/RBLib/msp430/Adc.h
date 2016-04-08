///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ���
//	MSP430
//
// ���� �������������� ������ ������ �������� � ������������ ������ ���� ���
// �� ���� ����� ������� ��� ������������� ����������.
// ���������������� ������ ������ ��� ���������� ������� ���� �� ��������������.
// ���������� ���������� ���������� �������� ���������� �� ��������������.
// �������� ������� ��������� ����� ���������.
// ������� ������ ���������������� �����������.
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef ADC_MSP430_H
#define ADC_MSP430_H

namespace Rblib
{
namespace Msp430
{
	// ����� ��� ���� �������� Msp430 ������ ���
	template<class RegisterDataType, class RegisterPointerType,
		class Control,
		class Mem0,
		class Mem1,
		class Mem2,
		class Mem3,
		class Mem4,
		class Mem5,
		class Mem6,
		class Mem7,
		class Mem8,
		class Mem9,
		class Mem10,
		class Mem11,
		class Mem12,
		class Mem13,
		class Mem14,
		class Mem15
	>
	class Adc
	{
	public:
		// ����� �������
		enum AdcChannels
		{
			AdcCh0 = 0,
			AdcCh1 = 1,
			AdcCh2 = 2,
			AdcCh3 = 3,
			AdcCh4 = 4,
			AdcCh5 = 5,
			AdcCh6 = 6,
			AdcCh7 = 7,
			AdcCh8 = 8,
			AdcCh9 = 9,
			AdcChTemperature = 10,		// ������������� ������
			AdcChAnalogVccHalf = 11,	// �������� Vcc
			AdcCh12 = 12,
			AdcCh13 = 13,
			AdcCh14 = 14,
			AdcCh15 = 15
		};
		// ����� ���������� �������� ����������
		enum AdcRefMax
		{
			AdcRefAnalogVcc = 0,
			AdcRefIntMax = 1,
			AdcRefExtMax = 2
		};
		enum AdcRefMin
		{
			AdcRefAnalogVss = 0,
			AdcRefIntMin = 1,
			AdcRefExtMin = 2
		};
		// ����� ���������� ������������
		enum AdcClockSource
		{
			AdcClockSourceModuleOscillator		= 0,
			AdcClockSourceAuxClock				= 1,
			AdcClockSourceMasterClock			= 2,
			AdcClockSourceSubsystemMasterClock	= 3
		};
		// �������� ���������
		enum AdcDivider
		{
			AdcDiv1 = 0,	// /1
			AdcDiv2 = 1,	// /2
			AdcDiv3 = 2,	// /3
			AdcDiv4 = 3,	// /4
			AdcDiv5 = 4,	// /5
			AdcDiv6 = 5,	// /6
			AdcDiv7 = 6,	// /7
			AdcDiv8 = 7		// /8
		};
		// �������� �������������
		enum AdcPreDivider
		{
			AdcPreDiv1 = 0,	// /1
			AdcPreDiv4 = 1	// /4
		};
		// �������� ������������ ��������� �������
		enum AdcSampleAndHoldTime
		{
			AdcSampleAndHoldTime4	= 0,
			AdcSampleAndHoldTime8	= 1,
			AdcSampleAndHoldTime16	= 2,
			AdcSampleAndHoldTime32	= 3,
			AdcSampleAndHoldTime64	= 4,
			AdcSampleAndHoldTime96	= 5,
			AdcSampleAndHoldTime128	= 6,
			AdcSampleAndHoldTime192	= 7,
			AdcSampleAndHoldTime256	= 8,
			AdcSampleAndHoldTime384	= 9,
			AdcSampleAndHoldTime512	= 10,
			AdcSampleAndHoldTime768	= 11,
			AdcSampleAndHoldTime1024 = 12
		};
	protected:
		// ������� �������� ������ ��������� �������� ���������� �� ���� � ����
		static inline unsigned char GetRef(AdcRefMax refMax, AdcRefMin refMin)
		{
			unsigned char res = refMax;
			if (refMin != AdcRefAnalogVss)
				res |= (1 << 2);
			return res;
		}
		// �������� �� ������ ������ �������� �� ������� ��������
		static inline RegisterDataType GetMemory(AdcChannels channel)
		{
			switch(channel)
			{
			case AdcCh0:
				return Mem0::GetResult();
			case AdcCh1:
				return Mem1::GetResult();
			case AdcCh2:
				return Mem2::GetResult();
			case AdcCh3:
				return Mem3::GetResult();
			case AdcCh4:
				return Mem4::GetResult();
			case AdcCh5:
				return Mem5::GetResult();
			case AdcCh6:
				return Mem6::GetResult();
			case AdcCh7:
				return Mem7::GetResult();
			case AdcCh8:
				return Mem8::GetResult();
			case AdcCh9:
				return Mem9::GetResult();
			case AdcChTemperature:
				return Mem10::GetResult();
			case AdcChAnalogVccHalf:
				return Mem11::GetResult();
			case AdcCh12:
				return Mem12::GetResult();
			case AdcCh13:
				return Mem13::GetResult();
			case AdcCh14:
				return Mem14::GetResult();
			case AdcCh15:
				return Mem15::GetResult();
			}

			return 0;
		}
	public:
		// ������� ������ ���
		static inline void Enable()
		{
		  	Control::Enable();
		}
		// �������� ������ ���
		static inline void Disable()
		{
			Control::Disable();
		}
		// ��������� �����
		static inline void SetupChannel(AdcChannels channel, AdcRefMax refMax, AdcRefMin refMin)
		{
			// ��������� �������� �������� ����������
			unsigned char reference = GetRef(refMax, refMin);
			// � ����������� �� ������ ����� � ������ ��������
			switch(channel)
			{
			case AdcCh0:
				Mem0::SetChannel(channel);
				Mem0::SetRefSource(reference);
				break;
			case AdcCh1:
				Mem1::SetChannel(channel);
				Mem1::SetRefSource(reference);
				break;
			case AdcCh2:
				Mem2::SetChannel(channel);
				Mem2::SetRefSource(reference);
				break;
			case AdcCh3:
				Mem3::SetChannel(channel);
				Mem3::SetRefSource(reference);
				break;
			case AdcCh4:
				Mem4::SetChannel(channel);
				Mem4::SetRefSource(reference);
				break;
			case AdcCh5:
				Mem5::SetChannel(channel);
				Mem5::SetRefSource(reference);
				break;
			case AdcCh6:
				Mem6::SetChannel(channel);
				Mem6::SetRefSource(reference);
				break;
			case AdcCh7:
				Mem7::SetChannel(channel);
				Mem7::SetRefSource(reference);
				break;
			case AdcCh8:
				Mem8::SetChannel(channel);
				Mem8::SetRefSource(reference);
				break;
			case AdcCh9:
				Mem9::SetChannel(channel);
				Mem9::SetRefSource(reference);
				break;
			case AdcChTemperature:
				Mem10::SetChannel(channel);
				Mem10::SetRefSource(reference);
				break;
			case AdcChAnalogVccHalf:
				Mem11::SetChannel(channel);
				Mem11::SetRefSource(reference);
				break;
			case AdcCh12:
				Mem12::SetChannel(channel);
				Mem12::SetRefSource(reference);
				break;
			case AdcCh13:
				Mem13::SetChannel(channel);
				Mem13::SetRefSource(reference);
				break;
			case AdcCh14:
				Mem14::SetChannel(channel);
				Mem14::SetRefSource(reference);
				break;
			case AdcCh15:
				Mem15::SetChannel(channel);
				Mem15::SetRefSource(reference);
				break;
			}
		}
		// ������� ��������� ��������
		static inline RegisterDataType GetSample(AdcChannels channel)
		{
			// ���� ������ ������ �� �������, �� ������ ������
			// ���� �� ���������, �� ����� ��������� � �������� ����� Busy
			if (!Control::IsEnabled())
				return 0;
			// ��������� ������ ������ ������
			Control::ConvertOneSample(channel);
			// ���������� ��� ����������
			return GetMemory(channel);
		}
		// ������ �������� ������������
		static inline void SetClockSource(AdcClockSource value)
		{
			Control::SetClockSource(value);
		}
		// ���������� �������� ��������� �������
		static inline void SetDivider(AdcDivider value)
		{
			Control::SetDivider(value);
		}
		// ���������� ������������ ��������� �������
		static inline void SetPreDivider(AdcPreDivider value)
		{
			Control::SetPreDivider(value);
		}
		// ��������� ������������ ��������� ��� ������� 0-7
		static inline void SetSampleAndHoldTimeFrom0To7(AdcSampleAndHoldTime value)
		{
			Control::SetSampleAndHoldTimeFrom0To7(value);
		}
		// ��������� ������������ ��������� ��� ������� 8-15
		static inline void SetSampleAndHoldTimeFrom8To15(AdcSampleAndHoldTime value)
		{
			Control::SetSampleAndHoldTimeFrom8To15(value);
		}
		// ��������� ������������ ��������� ��� ���� �������
		static inline void SetSampleAndHoldTime(AdcSampleAndHoldTime value)
		{
			Control::SetSampleAndHoldTime(value);
		}
	};
}
}

#endif