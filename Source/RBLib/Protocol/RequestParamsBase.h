
#ifndef REQUEST_PARAMS_BASE_H
#define REQUEST_PARAMS_BASE_H

#include "Protocol.h"

namespace Rblib
{
namespace Protocol
{

	/// \brief ��������� �������, ������� �����.
	/// \details ������� ����� ��� ������� ������������������ ���������� �������.
	/// ����� ���������� ���������� ������� ������ � ������� ������������ �������� ��� �������������.
	struct RequestParamsBase
	{
		/// ��������, �������� ������������� ���������.
		const ProtocolType Protocol;

		/// �����������.
		/// \param protocol ������������ ��������.
		RequestParamsBase(ProtocolType protocol) : Protocol(protocol)
		{
		}
	};

}
}

#endif // REQUEST_PARAMS_BASE_H
