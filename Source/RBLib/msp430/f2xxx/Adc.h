///////////////////////////////////////////////////////////////////////////////
//	Класс управления АЦП
//	MSP430F2xxx
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef ADC_MSP430F2XXX_H
#define ADC_MSP430F2XXX_H

#include "../Adc.h"
#include "../AdcParts.h"

namespace Rblib
{
namespace Msp430f2xxx
{
	// Тип значения регистра
	typedef unsigned short AdcRegisterDataType;
	// Тип адреса регистра
	typedef unsigned int  AdcRegisterAddressType;
	
	typedef Msp430::AdcControl<AdcRegisterDataType, AdcRegisterAddressType, 0x01A0, 0x01A2, Msp430::AdcControl0Register, Msp430::AdcControl1Register> AdcControl;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x0080, 0x0140, Msp430::AdcMemoryControlRegister> AdcMem0;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x0081, 0x0142, Msp430::AdcMemoryControlRegister> AdcMem1;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x0082, 0x0144, Msp430::AdcMemoryControlRegister> AdcMem2;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x0083, 0x0146, Msp430::AdcMemoryControlRegister> AdcMem3;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x0084, 0x0148, Msp430::AdcMemoryControlRegister> AdcMem4;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x0085, 0x014A, Msp430::AdcMemoryControlRegister> AdcMem5;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x0086, 0x014C, Msp430::AdcMemoryControlRegister> AdcMem6;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x0087, 0x014E, Msp430::AdcMemoryControlRegister> AdcMem7;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x0088, 0x0150, Msp430::AdcMemoryControlRegister> AdcMem8;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x0089, 0x0152, Msp430::AdcMemoryControlRegister> AdcMem9;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x008A, 0x0154, Msp430::AdcMemoryControlRegister> AdcMem10;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x008B, 0x0156, Msp430::AdcMemoryControlRegister> AdcMem11;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x008C, 0x0158, Msp430::AdcMemoryControlRegister> AdcMem12;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x008D, 0x015A, Msp430::AdcMemoryControlRegister> AdcMem13;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x008E, 0x015C, Msp430::AdcMemoryControlRegister> AdcMem14;
	typedef Msp430::AdcMemory<AdcRegisterDataType, AdcRegisterAddressType, 0x008F, 0x015E, Msp430::AdcMemoryControlRegister> AdcMem15;

	// АЦП для платформ msp430f2xxx
	class Adc : public Msp430::Adc <AdcRegisterDataType, AdcRegisterAddressType,
			AdcControl,
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