///////////////////////////////////////////////////////////////////////////////
//	Реализация функции устройтсва
//	Камера
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_CAMERA_IMPLEMENTER_H
#define PROTOCOL_G1_FUNCTION_CAMERA_IMPLEMENTER_H

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
namespace Camera
{
	struct SelectPhotoInfoParams
	{
	public:
		unsigned long StartTime;
		unsigned long EndTime;
		unsigned long Count;		// возвращается
		bool WirelessRequest; // признак запроса по радио
		int RequesterId; // иденетификатор запросившего узла
		
		CallbackWrapper<SelectPhotoInfoParams> CompleteCallback;	// вызывается по завершении обработки запроса
	public:
		SelectPhotoInfoParams()
		{
			Reset();
		}
		
		void Reset()
		{
			StartTime = 0;
			EndTime = 0;
			Count = 0;
			WirelessRequest = false;
			RequesterId = -1;
			CompleteCallback.Reset();
		}
	};
	
	template
		<
			int eventId,								// код событий
			int maxDataSize,								// максимальный размер полезных пользовательских данных
			int maxParamsCount							// максимальное число параметров сенсора
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
		typedef CallbackWrapper<CaptureFormatCode, bool &> StartCaptureCallbackType;
		typedef CallbackWrapper<unsigned int &, unsigned int &> GetCurrentPhotoInfoCallbackType;
		typedef CallbackWrapper<float &> GetAmbientLightLevelCallbackType;
		typedef CallbackWrapper<float &> GetBacklightPowerLevelCallbackType;
		typedef CallbackWrapper<int, ParamCode *, ParamCode *, ParamValueType *, int, int &> GetParamsCallbackType;
		typedef CallbackWrapper<int, ParamCode *, ParamValueType *, bool *, int, int &> SetParamsCallbackType;
		typedef CallbackWrapper<ParamCode *, ParamValueType *, int, int &> GetChangedParamsCallbackType;
		typedef CallbackWrapper<SelectPhotoInfoParams> SelectPhotoInfoCallbackType;

	protected:
		unsigned char _messageData[MaxDataSize + 50];
		unsigned char _functionData[MaxDataSize + 20];
	public:
		FromFunctionCallbackType FromFunctionCallback;
		FromFunctionToNodeCallbackType FromFunctionToNodeCallback;
		ToFunctionToNodeCallbackType ToFunctionToNodeCallback;
		StartCaptureCallbackType StartCaptureCallback;
		GetCurrentPhotoInfoCallbackType GetCurrentPhotoInfoCallback;
		GetAmbientLightLevelCallbackType GetAmbientLightLevelCallback;
		GetBacklightPowerLevelCallbackType GetBacklightPowerLevelCallback;
		GetParamsCallbackType GetParamsCallback;
		SetParamsCallbackType SetParamsCallback;
		GetChangedParamsCallbackType GetChangedParamsCallback;
		SelectPhotoInfoCallbackType SelectPhotoInfoCallback;
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
					case EventCodePhotoCreated:
						{
							unsigned int id = 0;
							unsigned int size = 0;
							GetCurrentPhotoInfoCallback.Call(id, size);
							
							MessageCaptureFunished messageCaptureFunished;
							messageCaptureFunished.Id = id;
							messageCaptureFunished.Size = size;
							int messageDataLen = messageCaptureFunished.ToByteArray(_messageData, sizeof(_messageData));
								
							FromFunction(MessageCodeCaptureFinished, _messageData, messageDataLen);
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
					}
					
				}
				break;
			}
		}
		
		// Отправить сообщение от имени функции
		void FromFunction(MessageCode code, void *data, int dataCount)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			FromFunctionCallback.Call(Device::FunctionCodeCamera, code, _functionData, functionDataLen);
		}
		
		// Отправить сообщение от имени функции конкретному узлу
		void FromFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			FromFunctionToNodeCallback.Call(node, Device::FunctionCodeCamera, code, _functionData, functionDataLen, enqueued, packetId);
		}
		
		// Отправить сообщение для функции конкретному узлу
		void ToFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			ToFunctionToNodeCallback.Call(node, Device::FunctionCodeCamera, code, _functionData, functionDataLen, enqueued, packetId);
		}
		
		void SendSelectPhotoInfoResult(SelectPhotoInfoParams params)
		{
			MessageSelectPhotoInfoResult message(params.Count);
			int messageDataLen = message.ToByteArray(_messageData, sizeof(_messageData));
			if (params.WirelessRequest)
			{
				bool enqueued = false;
				int packetId = -1;
				FromFunctionToNode(params.RequesterId, MessageCodeSelectPhotoInfoResult, _messageData, messageDataLen, enqueued, packetId);
			}
			else
			{
				FromFunction(MessageCodeSelectPhotoInfoResult, _messageData, messageDataLen);
			}
		}
		
		static void SendSelectPhotoInfoResult(void *callbackParam, SelectPhotoInfoParams params)
		{
			Implementer *im = (Implementer *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->SendSelectPhotoInfoResult(params);
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
					case MessageCodeStartCapture:
						{
							MessageStartCapture message(command.Data.Data, command.Data.DataLen);
							
							bool result = false;
							StartCaptureCallback.Call(message.CaptureFormat, result);
							
							MessageStartCaptureResult messageStartCaptureResult;
							messageStartCaptureResult.Result = result ? CaptureStartResultOk : CaptureStartResultBusy;
							int messageDataLen = messageStartCaptureResult.ToByteArray(_messageData, sizeof(_messageData));
								
							FromFunction(MessageCodeStartCaptureResult, _messageData, messageDataLen);
						}
						break;
					case MessageCodeGetAmbientLightLevel:
						{
							float level = 0.0;
							GetAmbientLightLevelCallback.Call(level);
							
							FromFunction(MessageCodeAmbientLightLevel, &level, sizeof(level));
						}
						break;
					case MessageCodeGetBacklightPowerLevel:
						{
							float level = 0.0;
							GetBacklightPowerLevelCallback.Call(level);
							
							FromFunction(MessageCodeBacklightPowerLevel, &level, sizeof(level));
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
					case MessageCodeSelectPhotoInfo:
						{
							MessageSelectPhotoInfo message(command.Data.Data, command.Data.DataLen);
							if (!message.Valid())
								break;
							
							SelectPhotoInfoParams params;
							params.StartTime = message.TimeFrom;
							params.EndTime = message.TimeTo;
							params.WirelessRequest = wirelessRequest;
							params.RequesterId = requesterId;
							params.CompleteCallback.Set(SendSelectPhotoInfoResult, this);
							// делаем выборку из БД
							SelectPhotoInfoCallback.Call(params);
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