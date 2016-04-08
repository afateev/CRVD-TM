///////////////////////////////////////////////////////////////////////////////
//	Реализация функции устройтсва
//	Сейсмический датчик
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_SEISMICSENSOR_IMPLEMENTER_H
#define PROTOCOL_G1_FUNCTION_SEISMICSENSOR_IMPLEMENTER_H

#include "FunctionData.h"
#include "EventCode.h"
#include "../../../../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Function
{
namespace SeismicSensor
{
	template
		<
			int eventId,								// код событий сесмического датчика
			int maxDataSize,							// максимальный размер полезных пользовательских данных
			int maxSeismicInfoChannelsCount,			// макстимальное количество каналов сейсмической информации
			typename channelInfoTypeType,				// тип кода типа информации, передаваемого в канале
			int maxParamsCount							// максимальное число параметров сенсора
		>
	class Implementer
	{
	public:
		static const int EventId = eventId;
		static const int MaxDataSize = maxDataSize;
		static const int MaxSeismicInfoChannelsCount = maxSeismicInfoChannelsCount;
		typedef channelInfoTypeType ChannelInfoTypeType;
		static const int MaxParamsCount = maxParamsCount;
		static const int MaxAlarmParamsCount = 32;
		typedef CallbackListWrapper<2, unsigned char, unsigned char, unsigned char *, unsigned int> FromFunctionCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, unsigned char *, unsigned int, bool &, int &> FromFunctionToNodeCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, unsigned char *, unsigned int, bool &, int &> ToFunctionToNodeCallbackType;
		typedef CallbackWrapper<bool &, int &, ChannelInfoTypeType *, int> GetSeismicInfoOutputModeCallbackType;
		typedef CallbackWrapper<bool, int, ChannelInfoTypeType *, bool &, bool &> SetSeismicInfoOutputModeCallbackType;
		typedef CallbackWrapper<int, ParamCode *, ParamCode *, ParamValueType *, int, int &> GetParamsCallbackType;
		typedef CallbackWrapper<int, ParamCode *, ParamValueType *, bool *, int, int &> SetParamsCallbackType;
		typedef CallbackWrapper<ParamCode *, ParamValueType *, int, int &> GetChangedParamsCallbackType;
		typedef CallbackWrapper<AlarmParamCode *, MessageAlarm::ParamValueType *, int, int &> GetAlarmParamsCallbackType;
		typedef CallbackWrapper<AlarmParamCode *, MessageAlarm::ParamValueType *, int, int &> GetFrequentAlarmParamsCallbackType;
		typedef CallbackWrapper<> AutotuningStartCallbackType;
	protected:
		unsigned char _messageData[MaxDataSize + 50];
		unsigned char _functionData[MaxDataSize + 20];
	public:
		FromFunctionCallbackType FromFunctionCallback;
		FromFunctionToNodeCallbackType FromFunctionToNodeCallback;
		ToFunctionToNodeCallbackType ToFunctionToNodeCallback;
		GetSeismicInfoOutputModeCallbackType GetSeismicInfoOutputModeCallback;
		SetSeismicInfoOutputModeCallbackType SetSeismicInfoOutputModeCallback;
		GetParamsCallbackType GetParamsCallback;
		SetParamsCallbackType SetParamsCallback;
		GetChangedParamsCallbackType GetChangedParamsCallback;
		GetAlarmParamsCallbackType GetAlarmParamsCallback;
		GetFrequentAlarmParamsCallbackType GetFrequentAlarmParamsCallback;
		AutotuningStartCallbackType AutotuningStartCallback;
	public:
		Implementer()
		{
			Singletone<EventDispetcher>::Instance().AddHandler(EventId, EventDispetcher::EventFunctor(*this), false);
		}
		
		// Обработчик событий
		void operator()(const Event &event)
		{
			switch (event.Code)
			{
			case EventId:
				{
					EventCode eventCode = (EventCode)event.Param1;
					switch(eventCode)
					{
					case EventCodeAlarm:
						{
							AlarmParamCode ids[MaxAlarmParamsCount];
							MessageAlarm::ParamValueType values[MaxAlarmParamsCount];
							int actualCount = 0;
							GetAlarmParamsCallback.Call(ids, values, MaxAlarmParamsCount, actualCount);
							
							MessageAlarm messageAlarm;
							messageAlarm.Count = actualCount;
							messageAlarm.Ids = ids;
							messageAlarm.Values = values;
							
							int messageDataLen = messageAlarm.ToByteArray(_messageData, sizeof(_messageData));
							
							FromFunction(MessageCodeAlarm, _messageData, messageDataLen);
						}
						break;
					case EventCodeFrequentAlarm:
						{
							AlarmParamCode ids[MaxAlarmParamsCount];
							MessageFrequentAlarm::ParamValueType values[MaxAlarmParamsCount];
							int actualCount = 0;
							GetFrequentAlarmParamsCallback.Call(ids, values, MaxAlarmParamsCount, actualCount);
							
							MessageFrequentAlarm messageFrequentAlarm;
							messageFrequentAlarm.Count = actualCount;
							messageFrequentAlarm.Ids = ids;
							messageFrequentAlarm.Values = values;
							
							int messageDataLen = messageFrequentAlarm.ToByteArray(_messageData, sizeof(_messageData));
							
							FromFunction(MessageCodeFrequentAlarm, _messageData, messageDataLen);
						}
						break;
					case EventCodeParamsChanged:
						{
							ParamCode ids[MaxParamsCount];
							ParamValueType values[MaxParamsCount];
							int actualCount = 0;
							GetChangedParamsCallback.Call(ids, values, MaxParamsCount, actualCount);
								
							MessageParams messageParams;
							messageParams.Count = actualCount;
							messageParams.Ids = ids;
							messageParams.Values = values;
							int messageDataLen = messageParams.ToByteArray(_messageData, sizeof(_messageData));
								
							FromFunction(MessageCodeParamsChanged, _messageData, messageDataLen);
						}
						break;
					case EventCodeAutotuningComplete:
						{
							FromFunction(MessageCodeAutotuningComplete, 0, 0);
						}
						break;
					}
					
				}
				break;
			}
		}
		
		void SendSeismicInfo(unsigned char channelCount, unsigned int *firstItemXValue, unsigned int *deltaX, ChannelInfoTypeCode *infoType, unsigned int *itemsCount, unsigned short ** yValues)
		{
			if (channelCount < 1 || channelCount > MaxSeismicInfoChannelsCount || firstItemXValue == 0 || deltaX == 0 || infoType == 0 || itemsCount == 0 || yValues == 0)
			{
				return;
			}
			
			MessageSeismicInfo::ChannelDataStruct channelData[MaxSeismicInfoChannelsCount];
			
			for (int i = 0; i < channelCount; i++)
			{
				channelData[i].FirstItemXValue = firstItemXValue[i];
				channelData[i].XDelta = deltaX[i];
				channelData[i].InfoType = infoType[i];
				channelData[i].ItemsCount = itemsCount[i];
				
				if (channelData[i].ItemsCount > 0)
				{
					if (yValues[i] == 0)
					{
						return;
					}
					
					channelData[i].YValues = yValues[i];
				}
				else
				{
					channelData[i].YValues = 0;
				}
			}
			
			MessageSeismicInfo messageSeismicInfo;
			messageSeismicInfo.ChannelCount = channelCount;
			messageSeismicInfo.ChannelData = channelData;
			int messageDataLen = messageSeismicInfo.ToByteArray(_messageData, sizeof(_messageData));
							
			FromFunction(MessageCodeSeismicInfo, _messageData, messageDataLen);
		}
		
		void SendSignalLevel(float signalLevel)
		{
			MessageSignalLevel messageSignalLevel;
			messageSignalLevel.SignalLevel = signalLevel;
			int messageDataLen = messageSignalLevel.ToByteArray(_messageData, sizeof(_messageData));
							
			FromFunction(MessageCodeSignalLevel, _messageData, messageDataLen);
		}
		
		// Отправить сообщение от имени функции
		void FromFunction(MessageCode code, void *data, int dataCount)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			FromFunctionCallback.Call(Device::FunctionCodeSeismicSensor, code, _functionData, functionDataLen);
		}
		
		// Отправить сообщение от имени функции конкретному узлу
		void FromFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			FromFunctionToNodeCallback.Call(node, Device::FunctionCodeSeismicSensor, code, _functionData, functionDataLen, enqueued, packetId);
		}
		
		// Отправить сообщение для функции конкретному узлу
		void ToFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			ToFunctionToNodeCallback.Call(node, Device::FunctionCodeSeismicSensor, code, _functionData, functionDataLen, enqueued, packetId);
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
					case MessageCodeGetSeismicInfoOutputMode:
						{
							bool enabled = false;
							int channelCount = 0;
							ChannelInfoTypeType channelInfoType[MaxSeismicInfoChannelsCount];
							GetSeismicInfoOutputModeCallback.Call(enabled, channelCount, channelInfoType, sizeof(channelInfoType) / sizeof(ChannelInfoTypeType));
							
							MessageSeismicInfoOutputMode messageSeismicInfoOutputMode;
							messageSeismicInfoOutputMode.ChannelCount = 0;
							messageSeismicInfoOutputMode.ChannelInfoType = 0;
							int messageDataLen = messageSeismicInfoOutputMode.ToByteArray(_messageData, sizeof(_messageData));
							
							FromFunction(MessageCodeSeismicInfoOutputMode, _messageData, messageDataLen);
						}
						break;
					case MessageCodeSetSeismicInfoOutputMode:
						{
							MessageSetSeismicInfoOutputMode message(command.Data.Data, command.Data.DataLen);
							
							MessageSetSeismicInfoOutputModeResult::ErrorCode error = MessageSetSeismicInfoOutputModeResult::ErrorCodeOk;
							
							if (message.Valid())
							{
								bool enabled = message.Enabled;
								int channelCount = message.ChannelCount;
								ChannelInfoTypeType *channelInfoType = message.ChannelInfoType;
								bool tooManyChannelCount = false;
								bool invalidInfoType = false;
								
								SetSeismicInfoOutputModeCallback.Call(enabled, channelCount, channelInfoType, tooManyChannelCount, invalidInfoType);
								
								if (channelInfoType == 0)
								{
									error = MessageSetSeismicInfoOutputModeResult::ErrorCodeInvalidDataFormat;
								}
								
								if (invalidInfoType)
								{
									error = MessageSetSeismicInfoOutputModeResult::ErrorCodeTooManyChannelCount;
								}
								
								if (tooManyChannelCount)
								{
									error = MessageSetSeismicInfoOutputModeResult::ErrorCodeTooManyChannelCount;
								}
							}
							else
							{
								error = MessageSetSeismicInfoOutputModeResult::ErrorCodeInvalidDataFormat;
							}
							
							MessageSetSeismicInfoOutputModeResult messageSetSeismicInfoOutputModeResult;
							messageSetSeismicInfoOutputModeResult.Error = error;
							int messageDataLen = messageSetSeismicInfoOutputModeResult.ToByteArray(_messageData, sizeof(_messageData));
							
							FromFunction(MessageCodeSetSeismicInfoOutputModeResult, _messageData, messageDataLen);
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
					case MessageCodeAutotuningStart:
						{
							AutotuningStartCallback.Call();
							FromFunction(MessageCodeAutotuningStartResult, 0, 0);
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