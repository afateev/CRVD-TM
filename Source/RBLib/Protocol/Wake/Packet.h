///////////////////////////////////////////////////////////////////////////////
//	������ ������ WAKE
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_WAKE_PACKET_H
#define PROTOCOL_WAKE_PACKET_H

namespace Rblib
{
namespace Protocol
{
namespace Wake
{
#pragma pack(push, 1)
	
	template
		<
			int InternalBufferLen = 255
		>
	class Packet
	{
	public:
		static const unsigned char FrameStart = 0xC0;            // ������ �����
		static const unsigned char FrameEscape = 0xDB;           // ������ Escape-������������������
		static const unsigned char TransposedFrameStart = 0xDC;  // ��� ��������� FrameStart ������ Escape-������������������
		static const unsigned char TransposedFrameEscape = 0xDD; // ��� ��������� FrameEscape ������ Escape-������������������
		static const unsigned char CrcStartValue = 0xDE;         // ��������� �������� ����������� �����
	protected:
		unsigned char _data[InternalBufferLen];
		int _dataCount;
	public:
		// ����������� �� ������ ���� (����� ������ ���� �� ����������)
		Packet(void *data, unsigned char dataLen)
		{
			_dataCount = UnStuff((unsigned char *)data, dataLen, _data, InternalBufferLen);
		}
		
		// ����������� �� ���� ������� � ���� ������
		Packet(unsigned char command, void *data, unsigned char dataLen)
		{
			_dataCount = 0;
			int packetLen = 3 + dataLen + 1;
			
			if (InternalBufferLen < packetLen)
			{
				return;
			}
			
			_dataCount = packetLen;
			_data[0] = FrameStart;
			_data[1] = command & ~0x80;
			_data[2] = dataLen;
			if (dataLen > 0)
			{
				for (int i = 0; i < dataLen; i++)
				{
					_data[3 + i] = ((unsigned char *)data)[i];
				}
			}
			_data[packetLen - 1] = CalcCrc();
		}
		
		// ����������� �� ���� ������, ���� ������� � ���� ������
		Packet(unsigned char address, unsigned char command, void *data, unsigned char dataLen)
		{
			_dataCount = 0;
			int packetLen = 4 + dataLen + 1;
			
			if (InternalBufferLen < packetLen)
			{
				return;
			}
			
			_dataCount = packetLen;
			_data[0] = FrameStart;
			_data[1] =address | 0x80;
			_data[2] = command & ~0x80;
			_data[3] = dataLen;
			if (dataLen > 0)
			{
				for (int i = 0; i < dataLen; i++)
				{
					_data[4 + i] = ((unsigned char *)data)[i];
				}
			}
			_data[packetLen - 1] = CalcCrc();
		}
		
		// ����� ���� ������
		bool HaveFrameStart()
		{
			return _data[0] == FrameStart;
		}
		
		// ����� ����� ���� ������
		bool HaveAddress()
		{
			if (!HaveFrameStart())
			{
				return false;
			}

			if (_dataCount < 2)
			{
				return false;
			}

			return (_data[1] & 0x80) == 0x80;
        }

		// �����
		unsigned char Address()
		{
			if (!HaveAddress())
			{
				return 0;
			}

			return _data[1] & ~0x80;
        }

		// ����� ����� ���� �������
		bool HaveCommand()
		{
			int offset = CommandOffset();
			bool have = _dataCount >= offset + 1;
			if (have)
			{
				if ((_data[offset] & 0x80) != 0)
				{
					// ������� ���� �� ������, ������ �� ���������� ������ ������
					have = false;
				}
			}
			return have;
        }

		// ��� �������
		unsigned char Command()
        {
			if (!HaveCommand())
			{
				return 0;
			}

			int cmdOffset = CommandOffset();
			return _data[cmdOffset] & ~0x80;
        }

		// ����� ����� ���� ����� ������
		bool HaveDataLen()
		{
			int offset = DataLenOffset();
			return _dataCount >= offset + 1;
        }

		// ����� ������
		unsigned int DataLen()
		{
			if (!HaveDataLen())
			{
				return 0;
			}

			int offset = DataLenOffset();

            return _data[offset];
        }

		// ����� ����� ���� ������
		bool HaveData()
		{
			int offset = DataLenOffset() + 1;
			unsigned int dataLen = DataLen();

			return _dataCount >= offset + dataLen && dataLen > 0;
        }

		// ���������� ���� ������
		unsigned char * Data()
		{
			if (!HaveData())
			{
				return 0;
			}

			int offset = DataOffset();

			return &_data[offset];
        }

		// ����� ����� ���� ����������� �����
		bool HaveCrc()
		{
			int offset = CrcOffset();
			return _dataCount >= offset + 1;
        }

		// ���������� ���� ����������� �����
		unsigned char Crc()
		{
			if (!HaveCrc())
			{
				return 0;
			}

			int offset = CrcOffset();

			return _data[offset];
        }

		// ���������� ���� ����������� ����� ��������� � ��������� ����������� ������
		bool CrcValid()
		{
			unsigned char expected = CalcCrc();
			return Crc() == expected;
        }
		
		// �������� ������������� ������ � ���� ������� ���� (�� ����������)
		int ToByteArray(void *dst, unsigned char dstSize)
		{
			if (!dst)
			{
				return 0;
			}

			if (dstSize < 1)
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			
			// ������ ���� ��� ���������
			dstPtr[0] = _data[0];
			
			unsigned char dstIndex = 1;
			
			// �� �������� ���������� �� ����� ������� �����
			for (int i = 1; i < _dataCount; i++)
			{
				unsigned char tmp[2];
				// �������� �� ��������� ������
				unsigned char stuffed = Stuff(_data[i], tmp);
				
				// �������� �� ����� �� �������� ������� dst
				if (dstIndex + stuffed >= dstSize)
				{
					return 0;
				}
				
				// �������� � �������� ����� ��������� ���������
				for (int j = 0; j < stuffed; j++)
				{
					dstPtr[dstIndex] = tmp[j];
					dstIndex++;
				}
			}
			
			return dstIndex;
        }
	protected:
		// �������� ���� ������� � �������� ������� ������
		int CommandOffset()
		{
			int res = 1;

			if (HaveAddress())
			{
				res++;
			}

			return res;
        }

		// �������� ���� ����� ������ � �������� ������� ������
		int DataLenOffset()
		{
			return CommandOffset() + 1;
		}

		// �������� ������ ���� ������ � �������� ������� ������
		int DataOffset()
		{
			if (!HaveData())
			{
				return 0;
			}

			return DataLenOffset() + 1;
        }

		// �������� ���� ����������� ����� � �������� ������� ������
		int CrcOffset()
		{
			return DataLenOffset() + DataLen() + 1;
		}
		
		// ��������� ����������� ����� �� �������� ����������� ������
		unsigned char CalcCrc()
		{
			unsigned char crc = CrcStartValue;

			for (int i = 0; i < _dataCount - 1; i++)
			{
				crc = GetNextCrcValue(crc, _data[i]);
			}

			return crc;
        }
		
		// ��������� �������� ����������� �����, �� ������ ����������� �������� � �������� ������ �����
        unsigned char GetNextCrcValue(unsigned char currentCrcValue, unsigned char nextByte)
		{
			unsigned char crc = currentCrcValue;
			unsigned char b = nextByte;

			for (unsigned char i = 0; i < 8; i++)
			{
				if (((b ^ crc) & 1) != 0)
				{
					crc = ((crc ^ 0x18) >> 1) | 0x80;
				}
				else
				{
					crc = (crc >> 1) & 0x7F;
				}
				b >>= 1;
			}

			return crc;
        }
		
		// �������� ������ �����
        unsigned char Stuff(unsigned char data, void *dst)
        {
			if (!dst)
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			
			switch (data)
			{
				case FrameStart:
					{
						dstPtr[0] = FrameEscape;
						dstPtr[0] = TransposedFrameStart;
						return 2;
					}
				case FrameEscape:
					{
						dstPtr[0] = FrameEscape;
						dstPtr[0] = TransposedFrameEscape;
						return 2;
					}
				default:
					{
						dstPtr[0] = data;
						return 1;
					}
            }
        }
		
		// ���������� ������������������ ����
		unsigned int UnStuff(unsigned char *data, unsigned int dataCount, unsigned char *res, unsigned char resSize)
        {
            if (data == 0 || dataCount < 1 || res == 0 || resSize < 1)
			{
				return 0;
			}
			
			// ������� ����������� ����
			unsigned char resCount = 0;
			
			// ������� ����, ��� ��������� �������������� ���� �������� ��������� Escape-������������������
			bool nextEscaped = false;
			
			for (int i = 0; i < dataCount; i++)
			{
				unsigned char b = data[i];

				if (!nextEscaped)
				{
					switch (b)
					{
					// ������ �����
					case FrameStart:
						{
							// ���� ������ ������ ��� ������ ���������
							if (resCount > 0)
							{
								// �� �� ���� ����� ����������
								return resCount;
							}
							if (resCount < resSize)
							{
								res[resCount] = b;
								resCount++;
							}
						}
						break;
					// ������ Escape-������������������
					case FrameEscape:
						{
							nextEscaped = true;
						}
						break;
					// ������� ����
					default:
						{
							if (resCount < resSize)
							{
								res[resCount] = b;
								resCount++;
							}
						}
						break;
					}
				}
				else
				{
					switch (b)
					{
					// ���������� ���� FrameStart
					case TransposedFrameStart:
						{
							if (resCount < resSize)
							{
								res[resCount] = FrameStart;
								resCount++;
							}
							nextEscaped = false;
						}
						break;
					// ���������� ���� FrameEscape
					case TransposedFrameEscape:
						{
							if (resCount < resSize)
							{
								res[resCount] = FrameEscape;
								resCount++;
							}
							nextEscaped = false;
						}
						break;
					// �� ������ ������ Escape-�������������������
					default:
						{
							//TODO �������� ������
						}
					}
				}
			}

            // ���������� ���-�� ������������� ����
			return resCount;
		}
	};
	
#pragma pack (pop)
}
}
}

#endif