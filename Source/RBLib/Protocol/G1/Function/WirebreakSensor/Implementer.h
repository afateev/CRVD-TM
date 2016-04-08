///////////////////////////////////////////////////////////////////////////////
//	���������� ������� ����������
//	�������� ������
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_WIREBREAKSENSOR_IMPLEMENTER_H
#define PROTOCOL_G1_FUNCTION_WIREBREAKSENSOR_IMPLEMENTER_H

#include "FunctionData.h"
#include "../../../../EventDispetcher.h"
#include "../../../../Utils/Singletone.h"
#include "../../../../Utils/CallbackWrapper.h"
#include "../../../../Utils/CallbackListWrapper.h"
#include "EventCode.h"

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace WirebreakSensor
{
	template
		<
			int eventId,								// ��� ������� ������
			int maxDataSize								// ������������ ������ �������� ���������������� ������
		>
	class Implementer
	{
	public:
		static const int EventId = eventId;
		static const int MaxDataSize = maxDataSize;
		typedef CallbackListWrapper<2, unsigned char, unsigned char, unsigned char *, unsigned int> FromFunctionCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, unsigned char *, unsigned int, bool &, int &> FromFunctionToNodeCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, unsigned char *, unsigned int, bool &, int &> ToFunctionToNodeCallbackType;
		typedef CallbackWrapper<bool * &, int &> GetWireStateCallbackType;
	protected:
		unsigned char _messageData[MaxDataSize + 20];
		unsigned char _functionData[MaxDataSize + 20];
	public:
		FromFunctionCallbackType FromFunctionCallback;
		FromFunctionToNodeCallbackType FromFunctionToNodeCallback;
		ToFunctionToNodeCallbackType ToFunctionToNodeCallback;
		GetWireStateCallbackType GetWireStateCallback;
	public:
		Implementer()
		{
			Reset();
			Singletone<EventDispetcher>::Instance().AddHandler(EventId, EventDispetcher::EventFunctor(*this), false);
		}
		
		void Reset()
		{
		}
		
		// ���������� �������
		void operator()(const Event &event)
		{
			switch (event.Code)
			{
			case EventId:
				{
					EventCode eventCode = (EventCode)event.Param1;
					switch(eventCode)
					{
					case EventCodeWireStateChanged:
						{
							bool *state = 0;
							int count = 0;
							GetWireStateCallback.Call(state, count);
							
							MessageWireState messageWireState;
							messageWireState.Count = count;
							messageWireState.State = state;
							
							int messageDataLen = messageWireState.ToByteArray(_messageData, sizeof(_messageData));
							
							FromFunction(MessageCodeWireStateChanged, _messageData, messageDataLen);
						}
						break;
					}
				}
				break;
			}
		}
		
		// ��������� ��������� �� ����� �������
		void FromFunction(MessageCode code, void *data, int dataCount)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			FromFunctionCallback.Call(Device::FunctionCodeWirebreakSensor, code, _functionData, functionDataLen);
		}
		
		// ��������� ��������� �� ����� ������� ����������� ����
		void FromFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			FromFunctionToNodeCallback.Call(node, Device::FunctionCodeWirebreakSensor, code, _functionData, functionDataLen, enqueued, packetId);
		}
		
		// ��������� ��������� ��� ������� ����������� ����
		void ToFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			ToFunctionToNodeCallback.Call(node, Device::FunctionCodeWirebreakSensor, code, _functionData, functionDataLen, enqueued, packetId);
		}
		
		void ToFunction(unsigned char *data, unsigned int dataCount, bool wirelessRequest, int requesterId)
		{
			InputMessageFormat command(data, dataCount);
			if (command.Valid())
			{
				if (command.Data.Valid())
				{
					switch(command.Data.Code)
					{
					case MessageCodeGetWireState:
						{
							bool *state = 0;
							int count = 0;
							GetWireStateCallback.Call(state, count);
							
							MessageWireState messageWireState;
							messageWireState.Count = count;
							messageWireState.State = state;
							
							int messageDataLen = messageWireState.ToByteArray(_messageData, sizeof(_messageData));
							
							FromFunction(MessageCodeWireState, _messageData, messageDataLen);
						}
						break;
					}
				}
			}
		}
		
		static void ToFunction(void *objectPtr, unsigned char *data, unsigned int dataCount, bool wirelessRequest, int requesterId)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			Implementer *im = (Implementer *)objectPtr;
			im->ToFunction(data, dataCount, wirelessRequest, requesterId);
		}
	};
}
}
}
}
}

#endif