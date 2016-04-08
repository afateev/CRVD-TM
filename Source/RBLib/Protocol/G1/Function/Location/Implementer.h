///////////////////////////////////////////////////////////////////////////////
//	Реализация функции устройтсва
//	Служба геопозиционирования
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_FUNCTION_LOCATION_IMPLEMENTER_H
#define PROTOCOL_G1_FUNCTION_LOCATION_IMPLEMENTER_H

#include "FunctionData.h"
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
namespace Location
{
	template
		<
			int eventId,
			int maxDataSize								// максимальный размер полезных пользовательских данных
		>
	class Implementer
	{
	public:
		static const int EventId = eventId;
		static const int MaxDataSize = maxDataSize;
		
		typedef CallbackListWrapper<2, unsigned char, unsigned char, unsigned char *, unsigned int> FromFunctionCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, unsigned char *, unsigned int, bool &, int &> FromFunctionToNodeCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char, unsigned char *, unsigned int, bool &, int &> ToFunctionToNodeCallbackType;
		
		typedef CallbackWrapper<bool &>	GetStateCallbackType;											// Запрос состояния «вкл/откл»
		typedef CallbackWrapper<bool, bool &> SetStateCallbackType;										// Установка состояния «вкл/откл»
		typedef CallbackWrapper<bool &, float &,float &, float &> GetLocationCallbackType;				// Запрос текущего местоположения
		typedef CallbackWrapper<unsigned int &, unsigned int &> GetSatelliteInfoCallbackType;			// Запрос статистики о видимых спутниках
		typedef CallbackWrapper<int &, int &, int &, int &, int &, int &>	GetDateTimeCallbackType;	// Запрос даты/времени
		
	protected:
		unsigned char _messageData[MaxDataSize + 20];
		unsigned char _functionData[MaxDataSize + 20];
	public:
		FromFunctionCallbackType FromFunctionCallback;
		FromFunctionToNodeCallbackType FromFunctionToNodeCallback;
		ToFunctionToNodeCallbackType ToFunctionToNodeCallback;
		
		GetStateCallbackType GetStateCallback;
		SetStateCallbackType SetStateCallback;
		GetLocationCallbackType GetLocationCallback;
		GetSatelliteInfoCallbackType GetSatelliteInfoCallback;
		GetDateTimeCallbackType GetDateTimeCallback;
	public:
		Implementer()
		{
			Reset();
			Singletone<EventDispetcher>::Instance().AddHandler(EventId, EventDispetcher::EventFunctor(*this), false);
		}
		
		void Reset()
		{
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
						case EventCodeStateChanged:
						{	// Включили/выключили GPS
							_messageData[0] = event.Param2;
							
							FromFunction(MessageCodeStateChanged, _messageData, 1);
						}
						break;
						case EventCodeLocationNotify:
						{	// Уведомление о текущих координатах
							float latitude  = 0.0;
							float longitude = 0.0;
							float altitude  = 0.0;
							bool active = false;
							
							GetLocationCallback.Call(active, latitude, longitude, altitude);
							
							MessageLocation messageLocation;
							messageLocation.Active = active;
							messageLocation.Latitude  = latitude;
							messageLocation.Longitude = longitude;
							messageLocation.Altitude  = altitude;
							
							int messageDataLen = messageLocation.ToByteArray(_messageData, sizeof(_messageData));
							
							FromFunction(MessageCodeLocation, _messageData, messageDataLen);
						}
						break;
					}
				}
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
			
			FromFunctionCallback.Call(Device::FunctionCodeLocation, code, _functionData, functionDataLen);
		}
		
		// Отправить сообщение от имени функции конкретному узлу
		void FromFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			FromFunctionToNodeCallback.Call(node, Device::FunctionCodeLocation, code, _functionData, functionDataLen, enqueued, packetId);
		}
		
		// Отправить сообщение для функции конкретному узлу
		void ToFunctionToNode(unsigned char node, MessageCode code, void *data, int dataCount, bool & enqueued, int & packetId)
		{
			OutputMessageFormat message;
			message.Data.Code = code;
			message.Data.DataLen = dataCount;
			message.Data.Data = data;
			int functionDataLen = message.ToByteArray(_functionData, sizeof(_functionData));
			
			ToFunctionToNodeCallback.Call(node, Device::FunctionCodeLocation, code, _functionData, functionDataLen, enqueued, packetId);
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
					case MessageCodeGetDateTime:
						{
							int year = 1970;
							int month = 1;
							int dayOfMonth = 1;
							int hour = 0;
							int minute = 0;
							int second = 0;
							
							GetDateTimeCallback.Call(year, month, dayOfMonth, hour, minute, second);
							
							MessageDateTime messageDateTime;
							messageDateTime.Year = year;
							messageDateTime.Month = month;
							messageDateTime.DayOfMonth = dayOfMonth;
							messageDateTime.Hour = hour;
							messageDateTime.Minute = minute;
							messageDateTime.Second = second;
							
							int messageDataLen = messageDateTime.ToByteArray(_messageData, sizeof(_messageData));
								
							FromFunction(MessageCodeDateTime, _messageData, messageDataLen);
						}
						break;
					case MessageCodeGetLocation:
						{
							float latitude  = 0.0;
							float longitude = 0.0;
							float altitude  = 0.0;
							bool active = false;
							
							GetLocationCallback.Call(active, latitude, longitude, altitude);
							
							MessageLocation messageLocation;
							messageLocation.Active = active;
							messageLocation.Latitude  = latitude;
							messageLocation.Longitude = longitude;
							messageLocation.Altitude  = altitude;
							
							int messageDataLen = messageLocation.ToByteArray(_messageData, sizeof(_messageData));
							
							FromFunction(MessageCodeLocation, _messageData, messageDataLen);
						}
						break;
					case MessageCodeGetSatelliteInfo:
						{
							unsigned int satelliteVisibleCount = 0;
							unsigned int satelliteUsedCount = 0;
								
							GetSatelliteInfoCallback.Call(satelliteVisibleCount, satelliteUsedCount);
							
							MessageSatelliteInfo messageSatelliteInfo;
							messageSatelliteInfo.SatelliteVisibleCount = satelliteVisibleCount;
							messageSatelliteInfo.SatelliteUsedCount = satelliteUsedCount;
							
							int messageDataLen = messageSatelliteInfo.ToByteArray(_messageData, sizeof(_messageData));
							
							FromFunction(MessageCodeSatelliteInfo, _messageData, messageDataLen);
						}
						break;
					case MessageCodeGetState:
						{
							bool state = false;
							
							GetStateCallback.Call(state);
							
							MessageState messageState;
							messageState.State = state;
							
							int messageDataLen = messageState.ToByteArray(_messageData, sizeof(_messageData));
							
							FromFunction(MessageCodeState, _messageData, messageDataLen);
						}
						break;
					case MessageCodeSetState:
						{
							if (command.Data.Data != 0 && command.Data.DataLen >= 1)
							{
								bool state = (bool)((unsigned char *)command.Data.Data)[0];
								bool result = false;
								
								SetStateCallback.Call(state, result);
								
								_messageData[0] = (unsigned char)result;
								
								FromFunction(MessageCodeSetStateResult, _messageData, 1);
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