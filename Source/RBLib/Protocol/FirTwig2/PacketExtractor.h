///////////////////////////////////////////////////////////////////////////////
//	���������� �������
//	�������� FirTwig2
//
//	����� ExtractPackets �������� �� ����������� ������ �����
//	� �������� ��� ���� ������� ��������� ������ � �������,
//	������� ������������ ������������� ������.
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef PACKETEXTRACTOR_FIRTWIG2_H
#define PACKETEXTRACTOR_FIRTWIG2_H

#include "../PacketExtractorCallbackTypeProvider.h"
#include "PacketFormat.h"

#include <string.h>

// ��������� ����� ���� ������ ������ ����� �������.
// ������������� �� � ������� ������� ��������� �.�.
// ������� ������ ������� ��� �����, �� ���� ��������� � ������� ������ � ��������� ������� ��������� ����� ������
//#define INTENSIVE_SEARCH 

namespace Rblib
{
namespace Protocol
{
namespace FirTwig2
{
/// ����� ��������� ������ � ������� ��������� FirTwig2 �� ����������� ������
template<bool functorCallback = false>
class PacketExtractor : public PacketExtractorCallbackTypeProvider<functorCallback>
{
	/// ���������� ����� ��� ������ ��������� ������
	class HeadFld
	{
		unsigned char buf[3];
		unsigned char index;
		unsigned char count;
		
	public:	
		HeadFld()
		{
			index = 0;
			count = 0;
		}

		/// ����� ���������.
		/// \param byte ��������� �������� ����.
		/// \param data ������� ����� ��� ����������� ������, � ���� ������������ ����� ���������, ���� �������� modify = true.
		/// \param modify ��������� ����������� ��������� � �����, �� ������� ��������� data.
		/// \return @c true - ������ ��������� ������, @c false - ��������� �� ������.
		bool Check(unsigned char byte, unsigned char* data, bool modify = true)	
		{
			if(count < 3)
			{
				count++;
				buf[index] = byte;
				if(++index == 3)
					index = 0;
				return false;
			}
			unsigned char crc = 0x89 + buf[0] + buf[1] + buf[2];
			if(byte != crc)
			{
				buf[index] = byte;
				if(++index == 3)
					index = 0;
				return false;
			}
			if(modify)
			{
				for(char i = 0; i < 3; i++)
				{			
					data[i] = buf[index];
					if(++index == 3)
						index = 0;
				}			
				data[3] = crc;
			}			
			count = 0;
			return true;//��������� ������
		}	
	} _headFld;

public:
    
	/// ��� ������� ��������� ������ ��� ��������� ������ � �������.
    typedef typename PacketExtractorCallbackTypeProvider<functorCallback>::CallbackType PacketReadyCallback;
    
protected:	

	/// ���������� ����� ������� (
	unsigned char _buf[5 + 256]; //���������� ����� �������
	unsigned int _index; //��� �������
	
	enum StateParser
	{
		FindHeaderState = 0,
		FindDataState = 1,
		FindDataCrcState = 2,
	} _state; //��������� �������
	
	static const unsigned char _lengthData = 2;
	static const unsigned char _crcHead = 3;


public:
	//����������� � ��������� � �������
	PacketExtractor()
	{
		_state = FindHeaderState;
	}

	/// ������� ��������� ������� � ��������������� �� ����������� � �������.
	/// \param data L����� �� ��������� ������ UART'�.
	/// \param length ���������� ������ � ������.
	/// \param pushFunctor �������, ������� ������ ���� ������� ��� ��������� ����������� ������ � �������.
	void ExtractPackets(unsigned char* data, unsigned int length, PacketReadyCallback pushFunctor)
	{
		unsigned int ptr = 0;
		if(length == 0)
			return;
		
		while(1)
		{
			switch(_state)
			{
			case FindHeaderState://������� ���������
				if(_headFld.Check(data[ptr++], _buf))//���� ��������� ������
				{
					if(_buf[_lengthData] != 0)//���� ����� ���� ������ �� ����� ����
					{
						_state = FindDataState;
						_index = 4;
					}
					else
						pushFunctor(_buf, 4);
				}
				break;
				
			case FindDataState://������� ������
				{
				unsigned int len = _buf[_lengthData] - (_index - 4);//���������� ��������� ����
				unsigned int rem = length - ptr;//���������� ���������� ���� � ������� ������
				if(len > rem)
					len = rem;
				else
					_state = FindDataCrcState;
				memcpy(&(_buf[_index]), &(data[ptr]), len);
				_index += len;
				ptr += len;
				break;
				}

			case FindDataCrcState://������� ����������� ����� ������
				{
					unsigned char crc = _buf[_crcHead];
					unsigned char* pt = &(_buf[4]);//��������� �� ������ ���� ������
					for(unsigned char i = 0; i < _buf[_lengthData]; i++)
						crc += *pt++;
					
					if(crc == data[ptr])//���� ����������� ����� ����������
					{
						_buf[_index++] = crc;
						pushFunctor(_buf, _index);

#ifdef INTENSIVE_SEARCH
						ptr++;						
						_state = FindHeaderState;
#endif
					}					
#ifndef INTENSIVE_SEARCH
					ptr++;						
					_state = FindHeaderState;
#else					
					else
					{
						unsigned int p = 0;
						unsigned int repeatIndex = _index;
						unsigned int offset = 1;
						unsigned char* buf = &(_buf[1]);						

						_state = FindHeaderState;
						StateParser repeatState = FindHeaderState;
						
						while(1)
						{
							switch(repeatState)
							{
								case FindHeaderState://������� ���������
									if(_headFld.Check(buf[p++], buf, false))//���� ��������� ������
									{
										buf += p - 4;
										if(buf[_lengthData] != 0)//���� ����� ���� ������ �� ����� ����
										{
											repeatState = FindDataState;
											repeatIndex = 4;
										}
										else
										{
											pushFunctor(buf, 4);
											buf += 4;
											offset += p;
											p = 0;
											repeatIndex = 0;
										}
									}
									break;
									
								case FindDataState://������� ������
								{
									unsigned int len = buf[_lengthData] - (repeatIndex - 4);//���������� ��������� ����
									unsigned int rem = _index - (p + offset);//���������� ���������� ���� � ������
									if(len > rem)
										len = rem;
									else
										repeatState = FindDataCrcState;
									repeatIndex += len;
									p += len;
									break;
								}
								
								case FindDataCrcState://������� ����������� ����� ������
								{
									unsigned char crc = buf[_crcHead];
									unsigned char* pt = &(buf[4]);//��������� �� ������ ���� ������
									for(unsigned char i = 0; i < buf[_lengthData]; i++)
										crc += *pt++;
					
									if(crc == buf[repeatIndex++])//���� ����������� ����� ����������
									{
										pushFunctor(buf, repeatIndex);										
										buf += repeatIndex;
										offset += repeatIndex;
									}
									else
									{
										buf++;
										offset++;
									}
									p = 0;
									repeatIndex = 0;
									repeatState = FindHeaderState;
									break;
								}			
							}
							if((p + offset) >= _index)
							{
								if(repeatIndex != _index)
								{
									memmove(_buf, buf, repeatIndex);
									_index = repeatIndex;
									_state = repeatState;
								}
								break;
							}
						}
					}
#endif					
				}
				break;					
			}	
			
			if(ptr >= length)
				break;
		}
	}		
};

}
}
}

#endif // PACKETEXTRACTOR_FIRTWIG2_H