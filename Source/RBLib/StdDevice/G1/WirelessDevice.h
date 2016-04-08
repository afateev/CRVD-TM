///////////////////////////////////////////////////////////////////////////////
//	 ласс реализации работы с беспроводным устройством по протоколу G1
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef G1_WIRELESSDEVCIE_H
#define G1_WIRELESSDEVCIE_H

#include "../../Utils/CallbackWrapper.h"
#include "../../Protocol/G1/Device/PacketWireless.h"
#include "EventCode.h"
#include "Function.h"
#include "Functions/WirelessModem.h"
#include "Functions/TextLog.h"
#include "Functions/SeismicSensor.h"
#include "Functions/SubscriptionService.h"
#include "Functions/Camera.h"
#include "Functions/Storage.h"
#include "Functions/WirebreakSensor.h"
#include "Functions/Relay.h"
#include "Functions/ActionService.h"

namespace Rblib
{
namespace StdDevice
{
namespace G1
{
	template
		<
			int maxDataSize,
			int eventCode,
			int timerTickCountPerSecond
		>
	class WirelessDevice
	{
	public:
		enum State
		{
			StateOffline = 0,
			StateOnline,
			StateTxGetDeviceInfo,
			StateTxGetDeviceInfoWaitDelivery,
			StateRxDeviceInfoWait,
			StateDeviceInfoReceived
		};
		
		static const unsigned int MaxDataSize = maxDataSize;
		static const int EventCode = eventCode;
		static const int TimerTickCountPerSecond = timerTickCountPerSecond;
		static const int FunctionsCount = 9;
		
		typedef Protocol::G1::Device::PacketWireless<MaxDataSize> PacketType;
		typedef CallbackWrapper<unsigned char, unsigned char *, unsigned int, int, int &> SendDataCallbackType;
		typedef Function<EventCode, MaxDataSize> FunctionType;
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_TEXTLOG_DISABLE
		typedef Functions::TextLog<FunctionType, TimerTickCountPerSecond> FunctionTextLogType;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_WIRELESSMODEM_DISABLE
		typedef Functions::WirelessModem<FunctionType, TimerTickCountPerSecond> FunctionWirelessModemType;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_SEISMICSENSOR_DISABLE
		typedef Functions::SeismicSensor<FunctionType, TimerTickCountPerSecond> FunctionSeismicSensorType;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_SUBSCRIPTIONSERVICE_DISABLE
		typedef Functions::SubscriptionService<FunctionType, TimerTickCountPerSecond> FunctionSubscriptionServiceType;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_CAMERA_DISABLE
		typedef Functions::Camera<FunctionType, TimerTickCountPerSecond> FunctionCameraType;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_STORAGE_DISABLE
		typedef Functions::Storage<FunctionType, maxDataSize, TimerTickCountPerSecond> FunctionStorageType;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_WIREBREAKSENSOR_DISABLE
		typedef Functions::WirebreakSensor<FunctionType, TimerTickCountPerSecond> FunctionWirebreakSensorType;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_RELAY_DISABLE
		typedef Functions::Relay<FunctionType, TimerTickCountPerSecond> FunctionRelayType;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_ACTIONSERVICE_DISABLE
		typedef Functions::ActionService<FunctionType, TimerTickCountPerSecond> FunctionActionServiceType;
#endif

	protected:
		bool _online;
		int _id;
		State _state;
		// один на всех экземпл€р пакета дл€ конструировани€ и отправки, должен использоватьс€ только как временный буфер
		static PacketType _packet;
		// один на всех экземпл€р буфера дл€ отправки данных
		static unsigned char _outputPacketData[MaxDataSize];
		int _waitPacketId;
		int _waitTimeout;
		FunctionType *_functions[FunctionsCount];
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_TEXTLOG_DISABLE
		FunctionTextLogType _functionTextLog;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_WIRELESSMODEM_DISABLE
		FunctionWirelessModemType _functionWirelessModem;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_SEISMICSENSOR_DISABLE
		FunctionSeismicSensorType _functionSeismicSensor;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_SUBSCRIPTIONSERVICE_DISABLE
		FunctionSubscriptionServiceType _functionSubscriptionService;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_CAMERA_DISABLE
		FunctionCameraType _functionCamera;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_STORAGE_DISABLE
		FunctionStorageType _functionStorage;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_WIREBREAKSENSOR_DISABLE
		FunctionWirebreakSensorType _functionWirebreakSensor;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_RELAY_DISABLE
		FunctionRelayType _functionRelay;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_ACTIONSERVICE_DISABLE
		FunctionActionServiceType _functionActionService;
#endif
	public:
		SendDataCallbackType SendDataCallback;
	public:
		WirelessDevice()
		{
			_id = -1;
			Reset();
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_TEXTLOG_DISABLE			
			_functions[0] = &_functionTextLog;
#else
			_functions[0] = 0;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_WIRELESSMODEM_DISABLE
			_functions[1] = &_functionWirelessModem;
#else
			_functions[1] = 0;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_SEISMICSENSOR_DISABLE
			_functions[2] = &_functionSeismicSensor;
#else
			_functions[2] = 0;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_SUBSCRIPTIONSERVICE_DISABLE
			_functions[3] = &_functionSubscriptionService;
#else
			_functions[3] = 0;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_CAMERA_DISABLE
			_functions[4] = &_functionCamera;
#else
			_functions[4] = 0;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_STORAGE_DISABLE
			_functions[5] = &_functionStorage;
#else
			_functions[5] = 0;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_WIREBREAKSENSOR_DISABLE
			_functions[6] = &_functionWirebreakSensor;
#else
			_functions[6] = 0;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_RELAY_DISABLE
			_functions[7] = &_functionRelay;
#else
			_functions[7] = 0;
#endif
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_ACTIONSERVICE_DISABLE
			_functions[8] = &_functionActionService;
#else
			_functions[8] = 0;
#endif
		}
		
		void Reset()
		{
			bool onlineChanged = _online;
			_online = false;
			_state = StateOffline;
			
			ResetFunctions();
			
			if (onlineChanged)
			{
				Event e(EventCode, EventCodeDeviceOffline, _id);
				Singletone<EventDispetcher>::Instance().PushEvent(e);
			}
		}
		
		bool Online()
		{
			return _online;
		}
		
		void SetOnline()
		{
			bool onlineChanged = !_online;
			_online = true;
			
			if (onlineChanged)
			{
				_state = StateOnline;
				
				Event e(EventCode, EventCodeDeviceOnline, _id);
				Singletone<EventDispetcher>::Instance().PushEvent(e);
			}
		}
		
		int Id()
		{
			return _id;
		}
		
		void SetId(int id)
		{
			_id = id;
		}
		
		void SetGateId(int id)
		{
#ifndef RBLIB_PROTOCOL_G1_NETWORK_WIRELESSDEVICE_FUNCTION_SUBSCRIPTIONSERVICE_DISABLE
			_functionSubscriptionService.SetGateId(id);
#endif
		}
		
		void OnTick()
		{
			switch(_state)
			{
			case StateOffline:
				{
				}
				break;
			case StateOnline:
				{
					_state = StateTxGetDeviceInfo;
				}
				break;
			case StateTxGetDeviceInfo:
				{
					_waitTimeout = SecondsToTimerTicks(60);
					int deliveryTimeout = FunctionType::DefaultTimeout;
					_waitPacketId = SendCommand(Protocol::G1::Device::CommandGetDeviceInfo, 0, 0, deliveryTimeout);
					_state = StateTxGetDeviceInfoWaitDelivery;
				}
				break;
			case StateTxGetDeviceInfoWaitDelivery:
			case StateRxDeviceInfoWait:
				{
					_waitTimeout--;
					if (_waitTimeout < 1)
					{
						_state = StateTxGetDeviceInfo;
					}
				}
				break;
			case StateDeviceInfoReceived:
				{
				}
				break;
			}
			
			for (int i = 0; i < FunctionsCount; i++)
			{
				if (_functions[i] != 0)
				{
					_functions[i]->OnTick();
				}
			}
		}
		
		void OnTxComplete(int packetId, bool delivered)
		{
			bool waitComplete = packetId == _waitPacketId;
			// дождались завершени€ передачи
			if (waitComplete)
			{
				switch(_state)
				{
				case StateTxGetDeviceInfoWaitDelivery:
					{
						if (delivered)
						{
							_waitTimeout = SecondsToTimerTicks(5);
							_state = StateRxDeviceInfoWait;
						}
						else
						{
							_state = StateTxGetDeviceInfo;
						}
					}
					break;
				}
			}
			
			for (int i = 0; i < FunctionsCount; i++)
			{
				if (_functions[i] != 0)
				{
					_functions[i]->OnTxComplete(packetId, delivered);
				}
			}
		}
		
		void OnDataReceived(unsigned char *buffer, int dataCount)
		{
			_packet.FromByteArray(buffer, dataCount);
			unsigned char *data = _packet.Data();
			int dataLen =  _packet.DataLen();
			
			switch(_packet.Command())
			{
			case Protocol::G1::Device::CommandDeviceInfo:
				{
					if(data != 0 && dataLen >= 5)
					{
						
						unsigned char functionsCount = data[4];
						if (dataLen >= 5 + functionsCount)
						{
							typename Protocol::G1::Device::FunctionCode *functionsList = (typename Protocol::G1::Device::FunctionCode *)&data[4 + 1];
							OnFunctionsListReceieved(functionsList, functionsCount);
							
							switch(_state)
							{
							case StateRxDeviceInfoWait:
								{
									_state = StateDeviceInfoReceived;
								}
								break;
							}
						}
					}
				}
				break;
			case Protocol::G1::Device::CommandFromFunction:
				{
					if (data != 0 && dataLen >= 1)
					{
						typename Protocol::G1::Device::FunctionCode functionCode = (typename Protocol::G1::Device::FunctionCode )data[0];
						FunctionType *function = GetFunction(functionCode);
						if (function)
						{
							function->OnCommandFromFunction(data, dataLen);
						}
					}
				}
				break;
			}
		}
		
		void InitFunctions()
		{
			for (int i = 0; i < FunctionsCount; i++)
			{
				if (_functions[i] != 0)
				{
					_functions[i]->SetDeviceId(_id);
					_functions[i]->SendCommandToFunctionCallback.Set(SendCommandToFunction, this);
				}
			}
		}
		
		FunctionType *GetFunction(typename Protocol::G1::Device::FunctionCode code)
		{
			for (int i = 0; i < FunctionsCount; i++)
			{
				if (_functions[i] != 0)
				{
					if (code == _functions[i]->FunctionCode())
					{
						return _functions[i];
					}
				}
			}
			
			return 0;
		}
	protected:
		int SecondsToTimerTicks(int seconds)
		{
			return TimerTickCountPerSecond * seconds;
		}
		
		int SendCommand(Protocol::G1::Device::CommandCode command, void *data, unsigned int dataCount, int timeout)
		{
			_packet.Set(command, data, dataCount);
			int packetLen = _packet.ToByteArray(_outputPacketData, sizeof(_outputPacketData));
			int packetId = 0;
			SendDataCallback.Call(_id, _outputPacketData, packetLen, timeout, packetId);
			return packetId;
		}
		
		int SendCommandToFunction(unsigned char *data, unsigned int dataCount, int timeout)
		{
			return SendCommand(Protocol::G1::Device::CommandToFunction, data, dataCount, timeout);
		}
		
		static void SendCommandToFunction(void *objectPtr, unsigned char *data, unsigned int dataCount, int timeout, int &packetId)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			WirelessDevice *im = (WirelessDevice *)objectPtr;
			packetId = im->SendCommand(Protocol::G1::Device::CommandToFunction, data, dataCount, timeout);
		}
		
		void OnFunctionsListReceieved(typename Protocol::G1::Device::FunctionCode *functions, int count)
		{
			for (int i = 0; i < count; i++)
			{
				FunctionType *function = GetFunction(functions[i]);
				if (function)
				{
					function->SetEnabled();
				}
			}
		}
		
		void ResetFunctions()
		{
			for (int i = 0; i < FunctionsCount; i++)
			{
				if (_functions[i] != 0)
				{
					_functions[i]->Reset();
				}
			}
		}
	};
	
	template <int maxDataSize, int eventCode, int timerTickCountPerSecond>
		typename WirelessDevice<maxDataSize, eventCode, timerTickCountPerSecond>::PacketType WirelessDevice<maxDataSize, eventCode, timerTickCountPerSecond>::_packet;
	
	template <int maxDataSize, int eventCode, int timerTickCountPerSecond>
		unsigned char WirelessDevice<maxDataSize, eventCode, timerTickCountPerSecond>::_outputPacketData[WirelessDevice<maxDataSize, eventCode, timerTickCountPerSecond>::MaxDataSize];
}
}
}

#endif