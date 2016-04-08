///////////////////////////////////////////////////////////////////////////////
//	Фабрика классов батарей питания
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef BATTARYFACTORY_H
#define BATTARYFACTORY_H

#include "BattaryBase.h"
#include "BattaryVip1026.h"

namespace Rblib
{
namespace StdDevice
{

template<class i2cBus>
class BattaryFactory
{
public:
	static BattaryBase *Create(BattaryType battoryType)
	{
		switch(battoryType)
		{
		case BattaryTypeUnknown:
			return new(nothrow) BattaryBase;
		case BattaryTypeVip1026:
			return new(nothrow) BattaryVip1026<i2cBus>;
		default:
			return 0;
		}
	}
};

}
}

#endif