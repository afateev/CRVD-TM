///////////////////////////////////////////////////////////////////////////////
//	������ ���������� ������� ������� ������� ���
//	MSP430F5XXX
//
//	������ ������ ������ ��������� �� ������ ������������� ��� �� ����������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef ADC_PARTS_MSP430F5XXX_H
#define ADC_PARTS_MSP430F5XXX_H

#include "../AdcParts.h"

namespace Rblib
{
// ����� ��� ����������� ��� ��������� MSP430 ��������� f5xxx
namespace Msp430f5xxx
{
	// ��������� �������� ���������� 2
	struct AdcControl2Register
	{
		unsigned short ReferenceBurst				: 1; //
		unsigned short ReferenceOutputEnable		: 1; //
		unsigned short SamplingRate					: 1; //
		unsigned short SignedFormat					: 1; //
		unsigned short Resolution					: 2; //
		unsigned short Reserved0					: 1; //
		unsigned short TemperatureSensorDisable		: 1; //
		unsigned short Predivider					: 1; //
		unsigned short Reserved1					: 7; //
	};

	// ����� ��������� ���������� ���
	template <class RegisterDataType,			// ��� (������) �������� ��������
			class PointerType,					// ��� (������) �������� ������� ���������
			PointerType Control0RegisterAddr,	// ����� �������� 0 ����������
			PointerType Control1RegisterAddr,	// ����� �������� 1 ����������
			PointerType Control2RegisterAddr,	// ����� �������� 2 ����������
			class Control0RegisterStructure,	// ��������� �������� 0 ����������
			class Control1RegisterStructure,	// ��������� �������� 1 ����������
			class Control2RegisterStructure		// ��������� �������� 2 ����������
		>
	class AdcControl : 
		public Msp430::AdcControl<RegisterDataType, PointerType, Control0RegisterAddr, Control1RegisterAddr, Control0RegisterStructure, Control1RegisterStructure>
	{
	protected:
		// ������ ��������� ����������
		static volatile Control2RegisterStructure * const _adcControl2RegisterAddr;
	public:
		// ���������� ������������ ��������� �������
		static inline void SetPreDivider(unsigned short value)
		{
			_adcControl2RegisterAddr->Predivider = value;
		}
	};

	// ��������������� ����������� ����������
	template<class RegisterDataType, class PointerType, PointerType Control0RegisterAddr, PointerType Control1RegisterAddr, PointerType Control2RegisterAddr, class Control0RegisterStructure, class Control1RegisterStructure, class Control2RegisterStructure>
	volatile  Control2RegisterStructure* const AdcControl<RegisterDataType, PointerType, Control0RegisterAddr, Control1RegisterAddr, Control2RegisterAddr, Control0RegisterStructure, Control1RegisterStructure, Control2RegisterStructure>
		::_adcControl2RegisterAddr = reinterpret_cast<volatile  Control2RegisterStructure*>(Control2RegisterAddr);
}
}

#endif