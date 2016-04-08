///////////////////////////////////////////////////////////////////////////////
//	Формат данных при работе с функцией устройства
//	Служба действий
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_ACTIONSERVICE_FUNCTIONDATA_H
#define PROTOCOL_G1_FUNCTION_ACTIONSERVICE_FUNCTIONDATA_H

#include "../../Device/CommandParams.h"
#include "ReasonCode.h"

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace ActionService
{
	enum MessageCode
	{
		MessageCodeInvalid							= 0,
		MessageCodeGetSlaveList						= 1,
		MessageCodeSlaveList						= 2,
		MessageCodeAddSlave							= 3,
		MessageCodeAddSlaveResult					= 4,
		MessageCodeRemoveSlave						= 5,
		MessageCodeRemoveSlaveResult				= 6,
		MessageCodeSlaveListChanged					= 7,
		MessageCodeDoAction							= 8,
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
	
	// Выходной формат данных функции Модем
	class OutputMessageFormat : public Device::CommandFromFunctionParams<Device::FunctionCodeActionService, MessageFormat>
	{
	};
	
	// Входной формат данных функции Модем
	class InputMessageFormat : public Device::CommandToFunctionParams<Device::FunctionCodeActionService, MessageFormat>
	{
	public:
		typedef Device::CommandToFunctionParams<Device::FunctionCodeActionService, MessageFormat> Base;
	public:
		InputMessageFormat(unsigned char *data, int dataCount) : Base(data, dataCount)
		{
		}
	};
	
	class MessageSlaveList
	{
	public:
		unsigned char Count;
		unsigned char *List;
	public:
		MessageSlaveList()
		{
			Count = 0;
			List = 0;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(Count) + Count)
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			if (Count > 0 && List != 0)
			{
				dstPtr[dstIndex] = Count;
				dstIndex++;
				
				for (int i = 0; i < Count; i ++)
				{
					dstPtr[dstIndex] = List[i];
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
	
	class MessageAddSlave
	{
	public:
		unsigned char Count;
		unsigned char *List;
	protected:
		bool _valid;
	public:
		MessageAddSlave(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < sizeof(Count))
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			Count = src[0];
			
			if (dataCount < sizeof(Count) + Count)
			{
				return;
			}
			
			List = (unsigned char *)&src[1];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
	};
	
	class MessageRemoveSlave
	{
	public:
		unsigned char Count;
		unsigned char *List;
	protected:
		bool _valid;
	public:
		MessageRemoveSlave(void *data, int dataCount)
		{
			_valid = false;
			
			if (data == 0 || dataCount < sizeof(Count))
			{
				return;
			}
			
			unsigned char *src = (unsigned char *)data;
			
			Count = src[0];
			
			if (dataCount < sizeof(Count) + Count)
			{
				return;
			}
			
			List = (unsigned char *)&src[1];
			
			_valid = true;
		}
		
		bool Valid()
		{
			return _valid;
		}
	};

	class MessageDoAction
	{
	public:
		typedef ReasonCode ReasonCodeType;
	public:
		unsigned char InitiatorId;	// идентификатор инициатора действия
		ReasonCodeType ReasonCode;		// код причины выполнения действия
		unsigned int ReasonId;		// идентификатор причины выполнения действия
	protected:
		bool _valid;
	public:
		MessageDoAction()
		{
			InitiatorId = -1;
			ReasonCode = ReasonCodeUnknown;
			ReasonId = -1;
		}
		
		MessageDoAction(void *data, int dataCount)
		{
			_valid = false;
			
			// если команда пришла без параметров, считаем информацию о инициаторе неизвестной
			// но сообщение допустимым
			if (data == 0 || dataCount == 0)
			{
				InitiatorId = -1;
				ReasonCode = ReasonCodeUnknown;
				ReasonId = -1;
				_valid = true;
			}
			else
			{
				// если параметры есть и их нужное число, то заполняем информацию об инициаторе
				if (dataCount >= sizeof(InitiatorId) + sizeof(ReasonCode) + sizeof(ReasonId))
				{
					unsigned char *src = (unsigned char *)data;
					
					InitiatorId = *(unsigned char *)&src[0];
					ReasonCode = (ReasonCodeType)(*(unsigned char *)&src[sizeof(InitiatorId)]);
					ReasonId = *(unsigned int *)&src[sizeof(InitiatorId) + sizeof(ReasonCode)];
					
					_valid = true;
				}
			}
		}
		
		bool Valid()
		{
			return _valid;
		}
		
		int ToByteArray(void *dst, int dstSize)
		{
			if (dst == 0 || dstSize < sizeof(InitiatorId) + sizeof(ReasonCode) + sizeof(ReasonId))
			{
				return 0;
			}
			
			unsigned char *dstPtr = (unsigned char *)dst;
			int dstIndex = 0;
			
			dstPtr[dstIndex] = InitiatorId;
			dstIndex++;
			
			dstPtr[dstIndex] = ReasonCode;
			dstIndex++;
			
			dstPtr[dstIndex] = ReasonId;
			dstIndex++;
			dstPtr[dstIndex] = (ReasonId >> 8) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (ReasonId >> 16) & 0xFF;
			dstIndex++;
			dstPtr[dstIndex] = (ReasonId >> 24) & 0xFF;
			dstIndex++;

			return dstIndex;
		}
	};
}
}
}
}
}

#endif