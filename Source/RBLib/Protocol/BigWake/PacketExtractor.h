///////////////////////////////////////////////////////////////////////////////
//	���������� �������
//	�������� BigWAKE
//
//	����� ExtractPackets �������� �� ����������� ������ �����
//	� �������� ��� ���� ������� ��������� ������ � �������,
//	������� ������������ ������������� ������.
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef PACKETEXTRACTOR_BIGWAKE_H
#define PACKETEXTRACTOR_BIGWAKE_H

#include "../PacketExtractorCallbackTypeProvider.h"
#include "Packet.h"

namespace Rblib
{
namespace Protocol
{
namespace BigWake
{
	template<bool functorCallback = false, unsigned int bufferSize = 1050> // bufferSize ������� ������ ��� 1024 �� �������
	class PacketExtractor : public PacketExtractorCallbackTypeProvider<functorCallback>
	{
	public:
    	/// ��� ������� ��������� ������ ��� ��������� ������ � �������.
		typedef typename PacketExtractorCallbackTypeProvider<functorCallback>::CallbackType PacketReadyCallback;
		// ������ ����������� ������
		static const unsigned int BufferSize = bufferSize;
		// ������������ ������ ������
		static const unsigned int MaxPacketSize = BufferSize > 0xFFFF ? 0xFFFF : BufferSize;
		// ��� ������
		typedef Packet<MaxPacketSize> PacketType;
	protected:
		unsigned char _currentPacketSourceData[BufferSize];		// ����� ��� �������� ������ �� ����������
		unsigned int _currentPacketSourceDataCount;				// ������� ����� �� �������� ������
		PacketType packet;										// ������� ����������� �����
	public:
		// ����������� �� ���������
		PacketExtractor()
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
				if (b == PacketType::FrameStart)
				{
					// ���� ��� ���� ����� ������
					if (_currentPacketSourceDataCount > 1)
					{
						// �� �� ���� ������� �����������
						BreakPacket();
					}

					// ������ ���� ������
					CurrentPacketSourceDataAdd(b);
				}
				else
                {
                    // ���� ��� �� ������ �����, ������ FrameStart ��� �� ���� � ���� ����������� ����� ��� ������
                    if (_currentPacketSourceDataCount < 1)
                    {
                        continue;
                    }
                    
                    // ��������� ����
                    CurrentPacketSourceDataAdd(b);
					
					// �������� �� ������ ����������� ������ ��������� �����
					packet.FromByteArray(_currentPacketSourceData, _currentPacketSourceDataCount);
					// ����� ��������� ������
					bool complete = packet.HaveFrameStart();
					
					if (complete)
					{
						complete &= packet.HaveCommand();
					}

					if (complete)
					{
						complete &= packet.HaveDataLen();
					}

					if (complete && packet.DataLen() > 0)
					{
						complete &= packet.HaveData();
					}

					if (complete)
					{
						complete &= packet.HaveCrc();
					}
					
					if (complete)
					{
						if (packet.CrcValid())
						{
							if (pushFunctor)
							{
								pushFunctor(_currentPacketSourceData, _currentPacketSourceDataCount);
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
		void CurrentPacketSourceDataAdd(unsigned char b)
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