
#ifndef REQUEST_PARAMS_FIRTWIG2_H
#define REQUEST_PARAMS_FIRTWIG2_H

#include "../RequestParamsBase.h"
#include "Parser.h"

namespace Rblib
{
namespace Protocol
{
namespace FirTwig2
{

	/// \brief ��������� ������� � ������� FirTwig2
	struct RequestParams : public RequestParamsBase
	{
		/// �������� �������, ����������� � �������.
		const Parser::PacketSummaryInfo& PacketInfo;

		/// �����������.
		/// \details ������� ����� ���������������� ����� ��������� FirTwig2.
		/// \param packetInfo ��������� ���������� �������, ���������� �� �������.
		RequestParams(const Parser::PacketSummaryInfo& packetInfo)
		: PacketInfo(packetInfo)
		, RequestParamsBase(ProtocolFirTwig2)
		{
		}

	};

}
}
}

#endif // REQUEST_PARAMS_FIRTWIG2_H
