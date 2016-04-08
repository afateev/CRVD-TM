
#ifndef DISPATCH_PARAMS_BASE_H
#define DISPATCH_PARAMS_BASE_H

#include "Protocol.h"

namespace Rblib
{
namespace Protocol
{

	/// \brief ��������� �������� �������, ������� �����.
	/// \details ������� ����� ��� ������� ������������������ ���������� ������� �������.
	/// ����� ���������� ���������� ������� ������ � ������� ������������ �������� ��� �������������.
	struct DispatchParamsBase
	{
		/// ��������, �������� ������������� ���������.
		const ProtocolType Protocol;

		/// �����������.
		/// \param protocol ������������ ��������.
		DispatchParamsBase(ProtocolType protocol) : Protocol(protocol)
		{
		}
	};

}
}

#endif // DISPATCH_PARAMS_BASE_H
