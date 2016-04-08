///////////////////////////////////////////////////////////////////////////////
//	���������� �������
//	�������� FirTwig3
//
//	����� ExtractPackets �������� �� ����������� ������ �����
//	� �������� ��� ���� ������� ��������� ������ � �������,
//	������� ������������ ������������� ������.
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef PACKETEXTRACTOR_FIRTWIG3_H
#define PACKETEXTRACTOR_FIRTWIG3_H

#include "../PacketExtractorCallbackTypeProvider.h"

namespace Rblib
{
namespace Protocol
{
namespace FirTwig3
{
//--------- ���������� ������� FirTwig3 --------------------------------------
template<bool functorCallback = false>
class PacketExtractor : public PacketExtractorCallbackTypeProvider<functorCallback>
{
public:

	// ��� ������� ��������� ������ ��� ��������� ������ � �������.
    typedef typename PacketExtractorCallbackTypeProvider<functorCallback>::CallbackType PacketReadyCallback;
    
	// ������� ��������� ������� � ��������������� �� ����������� � �������
	// data - ������ �� ��������� ������ UART'�
	// length - ���������� ������ � ������
	// pushFunctor - �������, ������� ������ ���� ������� ��� ��������� ����������� ������ � �������
	void ExtractPackets(unsigned char* data, unsigned int length, PacketReadyCallback pushFunctor)
	{
	}
};

}
}
}

#endif // PACKETEXTRACTOR_FIRTWIG3_H