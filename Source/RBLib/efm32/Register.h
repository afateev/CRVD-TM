#ifndef EMF32_REGISTER_H
#define EMF32_REGISTER_H

namespace Rblib
{
namespace Efm32
{
namespace Core
{
	// ��� �������� ��������
	typedef unsigned int RegisterValueType;
	// ��� ������ ��������
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