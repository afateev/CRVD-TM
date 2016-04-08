///////////////////////////////////////////////////////////////////////////////
//	Поддерживаемые типы батарей питания
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef BATTARYTYPE_H
#define BATTARYTYPE_H

namespace Rblib
{
namespace StdDevice
{

enum BattaryType
{
	BattaryTypeUnknown	= 0,
	BattaryTypeVip1026	= 1		// ВИП-1026 и совместимые с ним
};

}
}

#endif