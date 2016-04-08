
#ifndef CLOCK_SYSTEM_REGISTERS_MSP430X2XX_H
#define CLOCK_SYSTEM_REGISTERS_MSP430X2XX_H

#include "../x1xxx,x2xxx/ClockSystemRegisters.h"

namespace Rblib
{
namespace Msp430x2xx
{
	namespace ClockSystemParts
	{
		/// ��������� �������� BCSCTL3.
		union ControlRegister3
		{
			struct 
			{
				/// ���� LFXT1OF.
				/// \memberof ControlRegister3
				unsigned char ExternalSource1Fault : 1;
				/// ���� XT2OF.
				/// \memberof ControlRegister3
				unsigned char ExternalSource2Fault : 1;
				/// ���� XCAPx.
				/// \memberof ControlRegister3
				unsigned char ExternalSource1Capacitor : 2;
				/// ���� LFXT1Sx.
				/// \memberof ControlRegister3
				unsigned char ExternalSource1Settings : 2;
				/// ���� XT2Sx.
				/// \memberof ControlRegister3
				unsigned char ExternalSource2Settings : 2;
			};
			struct 
			{
				/// ����������� ���� LFXT1OF + XT2OF.
				/// \memberof ControlRegister3
				unsigned char ExternalSourceFaults : 2;
				unsigned char NotUsed : 6;
			};
		};

	}
}
}

#endif // CLOCK_SYSTEM_REGISTERS_MSP430X2XX_H
