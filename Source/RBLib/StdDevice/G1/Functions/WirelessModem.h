///////////////////////////////////////////////////////////////////////////////
//	Класс реализации работы с функцией устройства "Модем" по протоколу G1
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef G1_FUNCTIONS_WIRELESS_MODEM_H
#define G1_FUNCTIONS_WIRELESS_MODEM_H

#include "..\Function.h"

namespace Rblib
{
namespace StdDevice
{
namespace G1
{
namespace Functions
{
	template
		<
			class Base,
			int timerTickCountPerSecond
		>
	class WirelessModem : public Base
	{
	public:

		virtual Protocol::G1::Device::FunctionCode FunctionCode()
		{
			return Protocol::G1::Device::FunctionCodeWirelessModem;
		}
		
	};

}
}
}
}

#endif

