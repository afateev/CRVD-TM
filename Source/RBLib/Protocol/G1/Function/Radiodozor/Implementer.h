///////////////////////////////////////////////////////////////////////////////
//	���������� ������� ����������
//	����������
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_RADIODOZOR_IMPLEMENTER_H
#define PROTOCOL_G1_FUNCTION_RADIODOZOR_IMPLEMENTER_H

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
namespace Radiodozor
{
	template
		<
			int eventId,								// ��� ������� ������
			int maxDataSize,							// ������������ ������ �������� ���������������� ������
			int maxParamsCount							// ������������ ����� ����������
		>
	class Implementer
	{
	public:
		static const int EventId = eventId;
		static const int MaxDataSize = maxDataSize;
		static const int MaxParamsCount = maxParamsCount;
		typedef CallbackListWrapper<2, unsigned char, unsigned char, unsigned char *, unsigned int> FromFunctionCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, unsigned char *, unsigned int, bool &, int &> FromFunctionToNodeCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, unsigned char *, unsigned int, bool &, int &> ToFunctionToNodeCallbackType;
		typedef CallbackWrapper<ModeCode &> GetModeCallbackType;
		typedef CallbackWrapper<ModeCode, bool &> SetModeCallbackType;
		typedef CallbackWrapper<int, ParamCode *, ParamCode *, ParamValueType *, int, int &> GetParamsCallbackType;
		typedef CallbackWrapper<int, ParamCode *, ParamValueType *, bool *, int, int &> SetParamsCallbackType;
	protected:
		unsigned char _messageData[MaxDataSize + 20];
		unsigned char _functionData[MaxDataSize + 20];
	public:
		FromFunctionCallbackType FromFunctionCallback;
		FromFunctionToNodeCallbackType FromFunctionToNodeCallback;
		ToFunctionToNodeCallbackType ToFunctionToNodeCallback;
		GetModeCallbackType GetModeCallback;
		SetModeCallbackType SetModeCallback;
		GetParamsCallbackType GetParamsCallback;
		SetParamsCallbackType SetParamsCallback;
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
					case EventCodeModeChanged:
						{
							_messageData[0] = event.Param2;
							
							FromFunction(MessageCodeModeChanged, _messageData, 1);
						}
						break;
					case EventCodeParamsChanged:
						{
							ParamCode ids[MaxParamsCount];
							ParamValueType values[MaxParamsCount];
							int actualCount = 0;
							GetParamsCallback.Call(0, 0, ids, values, MaxParamsCount, actualCount);
								
							MessageParams messageParams;
							messageParams.Count = actualCount;
							messageParams.Ids = ids;
							messageParams.Values = values;
							int messageDataLen = messageParams.ToByteArray(_messageData, sizeof(_messageData));
								
							FromFunction(MessageCodeParamsChanged, _messageData, messageDataLen);
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
			
			FromFunctionCallback.Call(Device::FunctionCodeRadiodozor, code, _functionData, functionDataLen);
		}
		
		// ��������� ��������� �� ����� ������� ����������� ����
		void FromFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			FromFunctionToNodeCallback.Call(node, Device::FunctionCodeRadiodozor, code, _functionData, functionDataLen, enqueued, packetId);
		}
		
		// ��������� ��������� ��� ������� ����������� ����
		void ToFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			ToFunctionToNodeCallback.Call(node, Device::FunctionCodeRadiodozor, code, _functionData, functionDataLen, enqueued, packetId);
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
					case MessageCodeGetMode:
						{
							ModeCode mode = ModeCodeUnknown;
							GetModeCallback.Call(mode);
							
							_messageData[0] = (unsigned char)mode;
							
							FromFunction(MessageCodeMode, _messageData, 1);
						}
						break;
					case MessageCodeSetMode:
						{
							if (command.Data.Data != 0 && command.Data.DataLen >= 1)
							{
								ModeCode mode = (ModeCode)((unsigned char *)command.Data.Data)[0];
								bool result = false;
								SetModeCallback.Call(mode, result);
								
								_messageData[0] = (unsigned char)result;
								
								FromFunction(MessageCodeSetModeResult, _messageData, 1);
							}
						}
						break;
					case MessageCodeGetParams:
						{
							MessageGetParams message(command.Data.Data, command.Data.DataLen);
							
							if (message.Valid())
							{
								ParamCode ids[MaxParamsCount];
								ParamValueType values[MaxParamsCount];
								int actualCount = 0;
								GetParamsCallback.Call(message.Count, message.Ids, ids, values, MaxParamsCount, actualCount);
								
								MessageParams messageParams;
								messageParams.Count = actualCount;
								messageParams.Ids = ids;
								messageParams.Values = values;
								int messageDataLen = messageParams.ToByteArray(_messageData, sizeof(_messageData));
								
								FromFunction(MessageCodeParams, _messageData, messageDataLen);
							}
						}
						break;
					case MessageCodeSetParams:
						{
							MessageSetParams message(command.Data.Data, command.Data.DataLen);
							
							if (message.Valid())
							{
								bool setResults[MaxParamsCount];
								int actualCount = 0;
								SetParamsCallback.Call(message.Count, message.Ids, message.Values, setResults, MaxParamsCount, actualCount);
								
								MessageSetParamsResult messageSetParamsResult;
								messageSetParamsResult.Count = actualCount;
								messageSetParamsResult.Ids = message.Ids;
								messageSetParamsResult.Results = setResults;
								int messageDataLen = messageSetParamsResult.ToByteArray(_messageData, sizeof(_messageData));
								
								FromFunction(MessageCodeSetParamsResult, _messageData, messageDataLen);
							}
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