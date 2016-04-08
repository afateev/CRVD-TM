///////////////////////////////////////////////////////////////////////////////
//	Реализация устройтсва в терминах протокола G1
//	Для проводных асинхронных каналов передачи данных
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_DEVICE_IMPLEMENTERWIRED_H
#define PROTOCOL_G1_DEVICE_IMPLEMENTERWIRED_H

#include "../../../Utils/CallbackWrapper.h"
#include "PacketWired.h"
#include "../../BigWake/PacketExtractor.h"

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Device
{
	template
		<
			int maxDataSize				// максимальный размер полезных пользовательских данных
		>
	class ImplementerWired
	{
	public:
		static const int MaxDataSize = maxDataSize;
		typedef PacketWired<MaxDataSize + 20> PacketType;
		typedef Rblib::Protocol::BigWake::PacketExtractor<false, MaxDataSize + 20> PacketExtractorType;
		
		typedef CallbackWrapper<unsigned char *, unsigned int> SendDataCallbackType;
		typedef CallbackWrapper<CommandDeviceInfoParams &> GetDeviceInfoCallbackType;
		typedef CallbackWrapper<unsigned int &, unsigned int &, unsigned int &, unsigned int &> GetSoftwareVersionCallbackType;
		typedef CallbackWrapper<unsigned char *, unsigned int, bool, int> ToFunctionCallbackType;
	protected:
		PacketExtractorType _packetExtractor;
		PacketType _packet;
		unsigned char _outputPacketData[MaxDataSize + 20];
		unsigned char _tmpData[MaxDataSize + 20];
		static ImplementerWired *_selfPtr;
	public:
		SendDataCallbackType SendDataCallback;
		GetDeviceInfoCallbackType GetDeviceInfoCallback;
		GetSoftwareVersionCallbackType GetSoftwareVersionCallback;
		ToFunctionCallbackType ToFunctionTextLogCallback;
		ToFunctionCallbackType ToFunctionWirelessModemCallback;
		ToFunctionCallbackType ToFunctionSeismicSensorCallback;
		ToFunctionCallbackType ToFunctionSubscriptionServiceCallback;
		ToFunctionCallbackType ToFunctionCameraCallback;
		ToFunctionCallbackType ToFunctionStorageCallback;
		ToFunctionCallbackType ToFunctionWirebreakSensorCallback;
		ToFunctionCallbackType ToFunctionRelayCallback;
		ToFunctionCallbackType ToFunctionRadiodozorCallback;
		ToFunctionCallbackType ToFunctionDiagnosticsCallback;
		ToFunctionCallbackType ToFunctionActionServiceCallback;
		ToFunctionCallbackType ToFunctionAlarmLogCallback;
		ToFunctionCallbackType ToFunctionClockCallback;
		ToFunctionCallbackType ToFunctionPowerMonitorCallback;
		ToFunctionCallbackType ToFunctionRadiobeaconCallback;
		ToFunctionCallbackType ToFunctionLocationCallback;
	public:
		ImplementerWired()
		{
			_selfPtr = this;
		}
		
		void OnDataReceived(unsigned char *data, unsigned int dataCount)
		{
			if (dataCount > 0)
			{
				_packetExtractor.ExtractPackets(data, dataCount, OnPacketReceivedCallback);
			}
		}
		
		void FromFunction(unsigned char function, unsigned char dataCode, unsigned char *functionData, unsigned int functionDataLen)
		{
			SendCommand(CommandFromFunction, functionData, functionDataLen);
		}

		static void FromFunction(void *objectPtr, unsigned char function, unsigned char dataCode, unsigned char *functionData, unsigned int functionDataLen)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			ImplementerWired *im = (ImplementerWired *)objectPtr;
			
			im->FromFunction(function, dataCode, functionData, functionDataLen);
		}
	protected:
		void SendCommand(CommandCode command, void *data, unsigned int dataCount)
		{
			_packet.Set(command, data, dataCount);
			int packetLen = _packet.ToByteArray(_outputPacketData, sizeof(_outputPacketData));
			
			SendDataCallback.Call(_outputPacketData, packetLen);
		}
		
		static void OnPacketReceivedCallback(unsigned char *data, unsigned int dataCount)
		{
			_selfPtr->OnPacketReceived(data, dataCount);
		}
		
		void OnPacketReceived(unsigned char *data, unsigned int dataCount)
		{
			_packet.FromByteArray(data, dataCount);
			
			switch(_packet.Command())
			{
			case CommandGetDeviceInfo:
				{
					CommandDeviceInfoParams devInfo;
					GetDeviceInfoCallback.Call(devInfo);
					
					unsigned char deviceInfoLen = devInfo.ToByteArray(_tmpData, sizeof(_tmpData));
					SendCommand(CommandDeviceInfo, _tmpData, deviceInfoLen);
				}
				break;
			case CommandToFunction:
				{
					if (_packet.Data() != 0 && _packet.DataLen() > 0)
					{
						FunctionCode functionCode = (FunctionCode) _packet.Data()[0];
						
						switch (functionCode)
						{
						case FunctionCodeTextLog:
							{
								ToFunctionTextLogCallback.Call(_packet.Data(), _packet.DataLen(), 0, -1);
							}
							break;
						case FunctionCodeWirelessModem:
							{
								ToFunctionWirelessModemCallback.Call(_packet.Data(), _packet.DataLen(), 0, -1);
							}
							break;
						case FunctionCodeSeismicSensor:
							{
								ToFunctionSeismicSensorCallback.Call(_packet.Data(), _packet.DataLen(), 0, -1);
							}
							break;
						case FunctionCodeSubscriptionService:
							{
								ToFunctionSubscriptionServiceCallback.Call(_packet.Data(), _packet.DataLen(), 0, -1);
							}
							break;
						case FunctionCodeCamera:
							{
								ToFunctionCameraCallback.Call(_packet.Data(), _packet.DataLen(), 0, -1);
							}
							break;
						case FunctionCodeStorage:
							{
								ToFunctionStorageCallback.Call(_packet.Data(), _packet.DataLen(), 0, -1);
							}
							break;
						case FunctionCodeWirebreakSensor:
							{
								ToFunctionWirebreakSensorCallback.Call(_packet.Data(), _packet.DataLen(), 0, -1);
							}
							break;
						case FunctionCodeRelay:
							{
								ToFunctionRelayCallback.Call(_packet.Data(), _packet.DataLen(), 0, -1);
							}
							break;
						case FunctionCodeRadiodozor:
							{
								ToFunctionRadiodozorCallback.Call(_packet.Data(), _packet.DataLen(), 0, -1);
							}
							break;
						case FunctionCodeDiagnostics:
							{
								ToFunctionDiagnosticsCallback.Call(_packet.Data(), _packet.DataLen(), 0, -1);
							}
							break;
						case FunctionCodeActionService:
							{
								ToFunctionActionServiceCallback.Call(_packet.Data(), _packet.DataLen(), 0, -1);
							}
							break;
						case FunctionCodeAlarmLog:
							{
								ToFunctionAlarmLogCallback.Call(_packet.Data(), _packet.DataLen(), 0, -1);
							}
							break;
						case FunctionCodeClock:
							{
								ToFunctionClockCallback.Call(_packet.Data(), _packet.DataLen(), 0, -1);
							}
							break;
						case FunctionCodePowerMonitor:
							{
								ToFunctionPowerMonitorCallback.Call(_packet.Data(), _packet.DataLen(), 0, -1);
							}
							break;
						case FunctionCodeRadiobeacon:
							{
								ToFunctionRadiobeaconCallback.Call(_packet.Data(), _packet.DataLen(), 0, -1);
							}
							break;
						case FunctionCodeLocation:
							{
								ToFunctionLocationCallback.Call(_packet.Data(), _packet.DataLen(), 0, -1);
							}
							break;
						}
					}
				}
				break;
			case CommandGetSoftwareVersion:
				{
					CommandSoftwareVersionParams softwareVerion;
					GetSoftwareVersionCallback.Call(softwareVerion.A, softwareVerion.B, softwareVerion.C, softwareVerion.D);
					
					unsigned char dataLen = softwareVerion.ToByteArray(_tmpData, sizeof(_tmpData));
					SendCommand(CommandSoftwareVersion, _tmpData, dataLen);
				}
				break;
			}
		}
	};
	
	template <int maxDataSize>
		ImplementerWired<maxDataSize> *ImplementerWired<maxDataSize>::_selfPtr = 0;
}
}
}
}

#endif