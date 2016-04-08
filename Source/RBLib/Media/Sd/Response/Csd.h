
#ifndef _MEDIA_SD_RESPONSE_CSD_H_
#define _MEDIA_SD_RESPONSE_CSD_H_

#include "Csd/v1.h"
#include "Csd/v2.h"

namespace Rblib
{
namespace Media
{
namespace Sd
{
namespace Response
{

#pragma pack(push, 1)
class Csd
{
public:

	inline unsigned long Size() const
	{
		// проверяем какой тип у структуры: v1 или v2
		// вызываем Size() у объекта соответствующего типа
		// если тип 1, то v2, если тип 0, то v1
		if (_v2.Type()) // можно вызвать _v1.Type(), разницы нет
			return _v2.Size();
		return _v1.Size();
	}

	inline bool Sdhc() const
	{
		// если тип 1, то структура типа v2 (пывышенная ёмкость), если тип 0, то тип v1 (обычная ёмкость)
		return _v2.Type(); // можно вызвать _v1.Type(), разницы нет
	}

private:

	union
	{
		CSD::V1 _v1;
		CSD::V2 _v2;
	};

};
#pragma pack (pop)

}
}
}
}

#endif // _MEDIA_SD_RESPONSE_CSD_H_
