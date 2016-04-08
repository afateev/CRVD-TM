
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
		// ��������� ����� ��� � ���������: v1 ��� v2
		// �������� Size() � ������� ���������������� ����
		// ���� ��� 1, �� v2, ���� ��� 0, �� v1
		if (_v2.Type()) // ����� ������� _v1.Type(), ������� ���
			return _v2.Size();
		return _v1.Size();
	}

	inline bool Sdhc() const
	{
		// ���� ��� 1, �� ��������� ���� v2 (���������� �������), ���� ��� 0, �� ��� v1 (������� �������)
		return _v2.Type(); // ����� ������� _v1.Type(), ������� ���
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
