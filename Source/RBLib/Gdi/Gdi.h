///////////////////////////////////////////////////////////////////////////////
//	Графический интерфейс
//	Предоставляет контекст рисования для указанного драйвера устройства
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GDI_H
#define GDI_H

#include "DeviceDriver.h"
#include "DeviceContext.h"

namespace Rblib
{
namespace Gdi
{
	template<class DeviceDriver>
	class Gdi
	{
	public:
		typedef DeviceContext<DeviceDriver> DeviceContext;
	protected:
		static DeviceContext _defaultContext;
	public:
		static DeviceContext *GetDc()
		{
			return &_defaultContext;
		}
	};
	
	template<class DeviceDriver>
		typename Gdi<DeviceDriver>::DeviceContext Gdi<DeviceDriver>::_defaultContext;
}
}

#endif