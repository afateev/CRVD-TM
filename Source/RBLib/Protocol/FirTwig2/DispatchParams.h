
#ifndef DISPATCH_PARAMS_FIRTWIG2_H
#define DISPATCH_PARAMS_FIRTWIG2_H

#include "../DispatchParamsBase.h"

namespace Rblib
{
namespace Protocol
{
namespace FirTwig2
{

	/// \brief ��������� �������� ������� � ������� FirTwig2
	struct DispatchParams : public DispatchParamsBase
	{
		/// ������� ����� ���������� �������.
		signed char NetAddr;
		/// ������ ��������.
		unsigned char SendMode;

		/// ��������� ������������ ������ ��������.
		enum SendMode
		{
			/// ��������� � ������� �� ��������, ������� � ������������ � ���������� ��������.
			SendModeStd = 0x00,
			/// ����� �������� � ����� �������� ��� �������.
			SendModeUrgent = 0x20,
			/// �������� ����� ���� �������, ��������. ���������� �� ���������� ����� ����� ����� ���������.
			SendModeOneShot = 0x40,
			/// ������������� ���������. ���������������� ����� �������� �� ������ � ���������� �������,
			/// ������ ����� �������� 3 ������� ������� ����� �������� �������� ��� ������ ������������.
			/// ���� � 3 ������� ������������� ���, �� ����� ��������� �� ������ � � ������� �� ������������.
			SendModeForced = 0x80
		};

		/// ���������������� ������� �����.
		static const signed char NetAddrNone = -1;

		/// ����������� �� ���������.
		/// \details ������� ����� ���������������� ����� ��������� FirTwig2.
		DispatchParams()
		: NetAddr(NetAddrNone)
		, SendMode(SendModeStd)
		, DispatchParamsBase(ProtocolFirTwig2)
		{
		}

		/// �����������.
		/// \details ������� ����� ���������������� ����� ��������� FirTwig2.
		/// \param netAddr ������� �����.
		DispatchParams(signed char netAddr)
			: NetAddr(netAddr)
			, SendMode(SendModeStd)
			, DispatchParamsBase(ProtocolFirTwig2)
		{
		}

		/// �����������.
		/// \details ������� ����� ���������������� ����� ��������� FirTwig2.
		/// \param netAddr ������� �����.
		/// \param sendMode ������ ��������.
		DispatchParams(signed char netAddr, unsigned char sendMode)
		: NetAddr(netAddr)
		, SendMode(sendMode)
		, DispatchParamsBase(ProtocolFirTwig2)
		{
		}

	};

}
}
}

#endif // DISPATCH_PARAMS_FIRTWIG2_H
