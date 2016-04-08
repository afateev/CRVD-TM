///////////////////////////////////////////////////////////////////////////////
//	Класс реализации работы с функцие устройтсва "Текстовый лог" по протоколу G1
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef G1_FUNCTIONS_TEXT_LOG_H
#define G1_FUNCTIONS_TEXT_LOG_H

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
	class TextLog : public Base
	{
	public:

		virtual Protocol::G1::Device::FunctionCode FunctionCode()
		{
			return Protocol::G1::Device::FunctionCodeTextLog;
		}
		
	};

}
}
}
}

#endif
