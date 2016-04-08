///////////////////////////////////////////////////////////////////////////////
//	������ ������ ��� ������ � �������� ����������
//	����
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_RELAY_FUNCTIONDATA_H
#define PROTOCOL_G1_FUNCTION_RELAY_FUNCTIONDATA_H

#include "../../Device/CommandParams.h"
#include "ParamCode.h"

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace Relay
{
	enum MessageCode
	{
		MessageCodeInvalid							= 0,
		MessageCodeGetRelayState					= 1,
		MessageCodeRelayState						= 2,
		MessageCodeSetRelayState					= 3,
		MessageCodeSetRelayStateResult				= 4,
		MessageCodeRelayStateChanged				= 5,
		MessageCodeGetParams 						= 6,
		MessageCodeParams							= 7,
		MessageCodeSetParams						= 8,
		MessageCodeSetParamsResult 					= 9,
		MessageCodeParamsChanged					= 10,

	};
	
	class MessageFormat
	{
	public:
		MessageCode Code;
		unsigned short DataLen;
		void *Data;
	protected:
		bool _valid;
	public:
		MessageFormat()
		{
			Code = MessageCodeInvalid;
			DataLen = 0;
			Data = 0;
			_valid = true;
		}
		
		int Size()
		{
			return sizeof(Code) + sizeof(DataLen) + sizeof(unsigned char) * DataLen;
		}
		
		bool Valid()
		{
			return _valid;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < (3 + DataLen))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			unsigned char *dataPtr = (unsigned char *)Data;
			
			int dstIndex = 0;
			
			dstPtr[dstIndex] = Code;
			dstIndex++;
			
			if (DataLen > 0 && Data != 0)
			{
				dstPtr[dstIndex] = DataLen;
				dstIndex++;
				dstPtr[dstIndex] = (DataLen >> 8) & 0xFF;
				dstIndex++;
				
				for (int i = 0; i < DataLen; i ++)
				{
					dstPtr[dstIndex] = dataPtr[i];
					dstIndex++;
				}
			}
			else
			{
				dstPtr[dstIndex] = 0;
				dstIndex++;
				dstPtr[dstIndex] = 0;
				dstIndex++;
			}
			
			return dstIndex;
		}
		
		void FromByteArray(unsigned char *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < 3)
			{
				return;
			}
			
			Code = (MessageCode) data[0];
			DataLen = data[1];
			DataLen += data[2] << 8;
			
			if (dataCount < Size())
			{
				return;
			}
			
			Data = &data[3];
			
			_valid = true;
		}
	};
	
	// �������� ������ ������ ������� �����
	class OutputMessageFormat : public Device::CommandFromFunctionParams<Device::FunctionCodeRelay, MessageFormat>
	{
	};
	
	// ������� ������ ������ ������� �����
	class InputMessageFormat : public Device::CommandToFunctionParams<Device::FunctionCodeRelay, MessageFormat>
	{
	public:
		typedef Device::CommandToFunctionParams<Device::FunctionCodeRelay, MessageFormat> Base;
	public:
		InputMessageFormat(unsigned char *data, int dataCount) : Base(data, dataCount)
		{
		}
	};
	
	//
	// ������ ����������
	//
	class MessageGetParams
	{
	public:
		unsigned char Count;
		ParamCode *Ids;
	protected:
		bool _valid;
	public:
		MessageGetParams(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < 1)
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			Count = src[0];
			
			if (dataCount < Count * sizeof(ParamCode) + sizeof(Count))
			{
				return;
			}
			
			Ids = (ParamCode *)&src[1];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
	};
	
	//
	// ����� ���������, ���������� ���� � �������� ����������
	//
	typedef float ParamValueType;
	
	class MessageParamsHolder
	{
	public:

		unsigned char Count;
		ParamCode *Ids;
		ParamValueType *Values;

	public:

		MessageParamsHolder()
		: _valid(false)
		, Count(0)
		, Ids(0)
		, Values(0)
		{
		}

		// �� ����� �����, ���������� ������� ����� (�� ������ �����, ��������)
		MessageParamsHolder(unsigned char *data, int dataCount) : _valid(false)
		{
			if (data == 0 || dataCount < 1)
			{
				return;
			}

			Count = data[0];

			if (dataCount < 1 + Count + (Count << 2))
			{
				return;
			}

			Ids = (ParamCode*) data + 1;
			Values = (ParamValueType*) (data + 1 + Count);

			_valid = true;

		}

		// �� ����� ��������� ������� ��������������� � �������� (��� ������������ ������ ����� ���������)
		MessageParamsHolder(ParamCode* ids, ParamValueType* values, unsigned char count)
		: _valid(true)
		, Count(count)
		, Ids(ids)
		, Values(values)
		{
		}

		inline bool Valid()
		{
			return _valid;
		}

		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Count) * (sizeof(ParamCode) + sizeof(ParamValueType)))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			if (Count > 0 && Ids != 0 && Values != 0)
			{
				dstPtr[dstIndex] = Count;
				dstIndex++;
				
				for (int i = 0; i < Count; i ++)
				{
					dstPtr[dstIndex] = Ids[i];
					dstIndex++;
				}
				
				for (int i = 0; i < Count; i ++)
				{
					ParamValueType value = Values[i];
					
					for (int j = 0; j < sizeof(ParamValueType); j++)
					{
						dstPtr[dstIndex] = ((unsigned char *)&value)[j];
						dstIndex++;
					}
				}
			}
			else
			{
				dstPtr[dstIndex] = 0;
				dstIndex++;
			}
			
			return dstIndex;
		}

	protected:

		bool _valid;
	};
	
	//
	// ���������, ��������� �� �������
	//
	class MessageParams : public MessageParamsHolder
	{
	public:

		MessageParams() : MessageParamsHolder()
		{
		}

		MessageParams(unsigned char *data, int dataCount) : MessageParamsHolder(data, dataCount)
		{
		}

		MessageParams(ParamCode* ids, ParamValueType* values, unsigned char count) : MessageParamsHolder(ids, values, count)
		{
		}
	};

	//
	// ������ ����������
	//
	class MessageSetParams : public MessageParamsHolder
	{
	public:

		MessageSetParams() : MessageParamsHolder()
		{
		}

		MessageSetParams(unsigned char *data, int dataCount) : MessageParamsHolder(data, dataCount)
		{
		}

		MessageSetParams(ParamCode* ids, ParamValueType* values, unsigned char count) : MessageParamsHolder(ids, values, count)
		{
		}
	};
	
	//
	// ��������� ��������� ����������
	//
	class MessageSetParamsResult
	{
	public:
		unsigned char Count;
		ParamCode *Ids;
		bool *Results;
	public:
		MessageSetParamsResult()
		{
			Count = 0;
			Ids = 0;
			Results = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Count) * (sizeof(ParamCode) + sizeof(bool)))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			if (Count > 0 && Ids != 0 && Results != 0)
			{
				dstPtr[dstIndex] = Count;
				dstIndex++;
				
				for (int i = 0; i < Count; i ++)
				{
					dstPtr[dstIndex] = Ids[i];
					dstIndex++;
				}
				
				for (int i = 0; i < Count; i ++)
				{
					dstPtr[dstIndex] = Results[i];
					dstIndex++;
				}
			}
			else
			{
				dstPtr[dstIndex] = 0;
				dstIndex++;
			}
			
			return dstIndex;
		}
	};	
}
}
}
}
}

#endif
