#ifndef STM32_REGISTER_H
#define STM32_REGISTER_H

#ifdef STM32
#include "stm32\MemoryMap.h"
#endif

namespace Rblib
{
namespace Stm32
{
namespace Core
{
	// Тип значения регистра
	typedef unsigned int RegisterValueType;
	// Тип адреса регистра
	typedef unsigned int RegisterAddressType;
}	
	
	template
		<
			Core::RegisterValueType Address,
			typename Struct
		>
	class Register
	{
	public:
		static volatile Struct * Ptr()
		{
			//return reinterpret_cast<volatile Struct*>(Address);
			volatile Struct * res = (volatile Struct *)(Address);
			return res;
		}
		
		static volatile Struct * Ptr(Core::RegisterValueType offset)
		{
			//return reinterpret_cast<volatile Struct*>(Address);
			volatile Struct * res = (volatile Struct *)(Address + offset);
			return res;
		}
	};
	
}
}

#endif