///////////////////////////////////////////////////////////////////////////////
//	���������� �������
//	�������� NMEA
//
//	����� ExtractPackets �������� �� ����������� ������ �����
//	� �������� ��� ���� ������� ��������� ������ � �������,
//	������� ������������ ������������� ������.
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef PACKETEXTRACTOR_NMEA_H
#define PACKETEXTRACTOR_NMEA_H

#include "../PacketExtractorCallbackTypeProvider.h"
#include "Packet.h"

namespace Rblib
{
namespace Protocol
{
namespace Nmea
{
	template<bool functorCallback = false>
	class PacketExtractor : public PacketExtractorCallbackTypeProvider<functorCallback>
	{
	public:
    	/// ��� ������� ��������� ������ ��� ��������� ������ � �������.
		typedef typename PacketExtractorCallbackTypeProvider<functorCallback>::CallbackType PacketReadyCallback;
		// ������ ����������� ������
		static const unsigned int BufferSize = 128;
		// ��� ������
		typedef Packet PacketType;
	protected:
		char _currentPacketSourceData[BufferSize];		// ����� ��� �������� ������
		unsigned int _currentPacketSourceDataCount;		// ������� ����� �� �������� ������
		PacketType packet;								// ������� ����������� �����
	public:
		// ����������� �� ���������
		PacketExtractor() : packet(0, 0)
		{
			BreakPacket();
		}
		
		// ������� ��������� ������� � ��������������� �� ����������� � �������
		// data - ������ �� ��������� ������ UART'�
		// length - ���������� ������ � ������
		// pushFunctor - �������, ������� ������ ���� ������� ��� ��������� ����������� ������ � �������
		void ExtractPackets(unsigned char* data, unsigned int length, PacketReadyCallback pushFunctor)
		{
			if (data == 0 || length < 1)
			{
				return;
			}
		
			for (unsigned int i = 0; i < length; i++)
			{
				unsigned char b = data[i];
				if (b == PacketType::PacketStart)
				{
					// ���� ��� ���� ����� ������
					if (_currentPacketSourceDataCount > 1)
					{
						// �� �� ���� ������� �����������
						BreakPacket();
					}

					// ������ ������ ������
					CurrentPacketSourceDataAdd(b);
				}
				else
                {
                    // ���� ��� �� ������ �������, ������ PacketStart ��� �� ���� � ���� ����������� ����� ��� ������
                    if (_currentPacketSourceDataCount < 1)
                    {
                        continue;
                    }
                    
                    // ��������� ������
                    CurrentPacketSourceDataAdd(b);
					
					// �������� �� ������ ����������� ������ ��������� �����
					packet = PacketType(_currentPacketSourceData, _currentPacketSourceDataCount);
					// ����� ��������� ������
					bool complete = packet.Complete();
					
					if (complete)
					{
						if (packet.CrcValid())
						{
							if (pushFunctor)
							{
								pushFunctor((unsigned char *)_currentPacketSourceData, _currentPacketSourceDataCount);
							}
						}
						else
						{
							// TODO ������ �����
							BreakPacket();
						}
						
						// �������� �����
						BreakPacket();
					}
                }
			}
		}
	protected:
		// ������ ������
		void BreakPacket()
		{
			_currentPacketSourceDataCount = 0;
		}
		// �������� ������ �� ���������� �����
		void CurrentPacketSourceDataAdd(char b)
		{
			if (_currentPacketSourceDataCount < BufferSize)
			{
				_currentPacketSourceData[_currentPacketSourceDataCount] = b;
				_currentPacketSourceDataCount++;
			}
		}
	};
}
}
}

#endif