///////////////////////////////////////////////////////////////////////////////
//	Класс управления АЦП
//	MSP430F5xxx
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef ADC_MSP430F5XXX_H
#define ADC_MSP430F5XXX_H

#include "../Adc.h"
#include "AdcParts.h"

namespace Rblib
{
namespace Msp430f5xxx
{
	// Тип значения регистра
	typedef unsigned short AdcRegisterDataType;
	// Тип адреса регистра
	typedef unsigned int  AdcRegisterAddressType;
	
	typedef Msp430f5xxx::AdcControl<AdcRegisterDataType, AdcRegisterAddressType, 0x0700, 0x0702, 0x0704, Msp430::AdcControl0Register, Msp430::AdcControl1Register, Msp430f5xxx::AdcControl2Register> AdcControlExtended;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x0710, 0x0720, Msp430::AdcMemoryControlRegister> AdcMem0;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x0711, 0x0722, Msp430::AdcMemoryControlRegister> AdcMem1;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x0712, 0x0724, Msp430::AdcMemoryControlRegister> AdcMem2;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x0713, 0x0726, Msp430::AdcMemoryControlRegister> AdcMem3;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x0714, 0x0728, Msp430::AdcMemoryControlRegister> AdcMem4;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x0715, 0x072A, Msp430::AdcMemoryControlRegister> AdcMem5;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x0716, 0x072C, Msp430::AdcMemoryControlRegister> AdcMem6;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x0717, 0x072E, Msp430::AdcMemoryControlRegister> AdcMem7;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x0718, 0x0730, Msp430::AdcMemoryControlRegister> AdcMem8;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x0719, 0x0732, Msp430::AdcMemoryControlRegister> AdcMem9;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x071A, 0x0734, Msp430::AdcMemoryControlRegister> AdcMem10;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x071B, 0x0736, Msp430::AdcMemoryControlRegister> AdcMem11;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x071C, 0x0738, Msp430::AdcMemoryControlRegister> AdcMem12;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x071D, 0x073A, Msp430::AdcMemoryControlRegister> AdcMem13;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x071E, 0x073C, Msp430::AdcMemoryControlRegister> AdcMem14;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x071F, 0x073E, Msp430::AdcMemoryControlRegister> AdcMem15;

	// АЦП для платформ msp430f5xxx
	class Adc : public Msp430::Adc <AdcRegisterDataType, AdcRegisterAddressType,
			AdcControlExtended,
			AdcMem0,
			AdcMem1,
			AdcMem2,
			AdcMem3,
			AdcMem4,
			AdcMem5,
			AdcMem6,
			AdcMem7,
			AdcMem8,
			AdcMem9,
			AdcMem10,
			AdcMem11,
			AdcMem12,
			AdcMem13,
			AdcMem14,
			AdcMem15
		>
	{

	};
}
}

#endif