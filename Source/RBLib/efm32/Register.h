#ifndef EMF32_REGISTER_H
#define EMF32_REGISTER_H

namespace Rblib
{
namespace Efm32
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
			return reinterpret_cast<volatile Struct*>(Address);
		}
	};
	
}
}

#endif