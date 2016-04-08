///////////////////////////////////////////////////////////////////////////////
//	Реализация устройтсва в терминах протокола G1
//	Для беспроводных каналов передачи данных
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PROTOCOL_G1_DEVICE_IMPLEMENTERWIRELESS_H
#define PROTOCOL_G1_DEVICE_IMPLEMENTERWIRELESS_H

#include "../../../Utils/CallbackWrapper.h"
#include "PacketWireless.h"

namespace Rblib
{
namespace Protocol
{
namespace G1
{
namespace Device
{
	template<class PacketType, bool functorCallback = false, unsigned int bufferSize = 1050> // bufferSize немного больше чем 1024 на обвеску
		class WirelessPacketExtractor : public Rblib::Protocol::PacketExtractorCallbackTypeProvider<functorCallback>
	{
	public:
    	/// Тип функции обратного вызова для помещения пакета в очередь.
		typedef typename PacketExtractorCallbackTypeProvider<functorCallback>::CallbackType PacketReadyCallback;
		
		static const unsigned int BufferSize = PacketType::BufferSize;
	protected:
		PacketType _packet;
	public:
		// функция выделения пакетов и автоматического их укладывания в очередь
		// data - данные из приемного буфера UART'а
		// length - количество данных в буфере
		// pushFunctor - функция, которая должна быть вызвана для помещения выделенного пакета в очередь
		void ExtractPackets(unsigned char* data, unsigned int length, PacketReadyCallback pushFunctor)
		{
			// пытаемся построить пакет
			_packet.FromByteArray(data, length);
			// валидация пакета
			if (_packet.Valid())
			{
				if (pushFunctor)
				{
					pushFunctor(data, length);
				}
			}
		}
		
		unsigned char *GetBufferPtr()
		{
			return _packet.GetBufferPtr();
		}
	};
	
	template
		<
			int maxDataSize				// максимальный размер полезных пользовательских данных
		>
	class ImplementerWireless
	{
	public:
		static const int MaxDataSize = maxDataSize;
		typedef PacketWireless<MaxDataSize> PacketType;
		typedef WirelessPacketExtractor<PacketType> PacketExtractorType;
		
		typedef CallbackWrapper<unsigned char, unsigned char, unsigned char *, unsigned int> SendDataByHimselfCallbackType;
		typedef CallbackWrapper<unsigned char, unsigned char *, unsigned int, bool&, int&> SendDataAsResponseCallbackType;
		typedef CallbackWrapper<CommandDeviceInfoParams &> GetDeviceInfoCallbackType;
		typedef CallbackWrapper<unsigned int &, unsigned int &, unsigned int &, unsigned int &> GetSoftwareVersionCallbackType;
		typedef CallbackWrapper<unsigned char *, unsigned int, bool, int> ToFunctionCallbackType;
	protected:
		PacketExtractorType _packetExtractor;
		PacketType _packet;
		unsigned char _outputPacketData[MaxDataSize];
		unsigned char _tmpData[MaxDataSize];
		static ImplementerWireless *_selfPtr;
		bool _sendNextCommandAsResponse;
		unsigned char _responseAddress;
	public:
		SendDataByHimselfCallbackType SendDataByHimselfCallback;
		SendDataAsResponseCallbackType SendDataAsResponseCallback;
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
		ImplementerWireless()
		{
			_selfPtr = this;
			_sendNextCommandAsResponse = false;
			_responseAddress = 0;
		}
		
		unsigned char *GetInternalBufferPtr()
		{
			return _packetExtractor.GetBufferPtr();
		}
		
		int GetInternalBufferSize()
		{
			return PacketExtractorType::BufferSize;
		}
		
		void OnDataReceived(unsigned char sourceAddress, unsigned char *data, unsigned int dataCount)
		{
			if (dataCount > 0)
			{
				// все ответы на разобранные команды, будут отправляться как ответы конкретному устройтсву
				_sendNextCommandAsResponse = true;
				_responseAddress = sourceAddress;
				_packetExtractor.ExtractPackets(data, dataCount, OnPacketReceivedCallback);
				// далше команды следует слать не как ответы, а как пакеты "по собственной инициативе"
				_sendNextCommandAsResponse = false;
				_responseAddress = 0;
			}
		}
		
		void FromFunction(unsigned char function, unsigned char dataCode, unsigned char *functionData, unsigned int functionDataLen)
		{
			bool enqueued;
			int packetId;
			SendCommand(function, dataCode, CommandFromFunction, functionData, functionDataLen, enqueued, packetId);
		}

		void FromFunctionToNode(unsigned char node, unsigned char function, unsigned char dataCode, unsigned char *functionData, unsigned int functionDataLen, bool& enqueued, int & packetId)
		{
			_sendNextCommandAsResponse = true;
			_responseAddress = node;
			SendCommand(function, dataCode, CommandFromFunction, functionData, functionDataLen, enqueued, packetId);
			_sendNextCommandAsResponse = false;
			_responseAddress = 0;
		}
		
		void ToFunctionToNode(unsigned char node, unsigned char function, unsigned char dataCode, unsigned char *functionData, unsigned int functionDataLen, bool& enqueued, int & packetId)
		{
			_sendNextCommandAsResponse = true;
			_responseAddress = node;
			SendCommand(function, dataCode, CommandToFunction, functionData, functionDataLen, enqueued, packetId);
			_sendNextCommandAsResponse = false;
			_responseAddress = 0;
		}

		static void FromFunction(void *objectPtr, unsigned char function, unsigned char dataCode, unsigned char *functionData, unsigned int functionDataLen)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			ImplementerWireless *im = (ImplementerWireless *)objectPtr;
			
			im->FromFunction(function, dataCode, functionData, functionDataLen);
		}
		
		static void FromFunctionToNode(void *objectPtr, unsigned char node, unsigned char function, unsigned char dataCode, unsigned char *functionData, unsigned int functionDataLen, bool & enqueued, int & packetId)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			ImplementerWireless *im = (ImplementerWireless *)objectPtr;

			im->FromFunctionToNode(node, function, dataCode, functionData, functionDataLen, enqueued, packetId);
		}
		
		static void ToFunctionToNode(void *objectPtr, unsigned char node, unsigned char function, unsigned char dataCode, unsigned char *functionData, unsigned int functionDataLen, bool & enqueued, int & packetId)
		{
			if (0 == objectPtr)
			{
				return;
			}
			
			ImplementerWireless *im = (ImplementerWireless *)objectPtr;

			im->ToFunctionToNode(node, function, dataCode, functionData, functionDataLen, enqueued, packetId);
		}
	protected:
		// для отправки команд от имени Device
		void SendCommand(CommandCode command, void *data, unsigned int dataCount)
		{
			_packet.Set(command, data, dataCount);
			int packetLen = _packet.ToByteArray(_outputPacketData, sizeof(_outputPacketData));
			
			// такие комады можно отправлять только в ответ на что-то иначе невозможно установить подписчика
			if (_sendNextCommandAsResponse)
			{
			  	bool enqueued;
				int packetId;
				SendDataAsResponseCallback.Call(_responseAddress, _outputPacketData, packetLen, enqueued, packetId);
			}
		}
		
		void SendCommand(unsigned char function, unsigned char dataCode, CommandCode command, void *data, unsigned int dataCount, bool& enqueued, int &packetId)
		{
			_packet.Set(command, data, dataCount);
			int packetLen = _packet.ToByteArray(_outputPacketData, sizeof(_outputPacketData));
			
			if (_sendNextCommandAsResponse)
			{
				SendDataAsResponseCallback.Call(_responseAddress, _outputPacketData, packetLen, enqueued, packetId);
			}
			else
			{
				SendDataByHimselfCallback.Call(function, dataCode, _outputPacketData, packetLen);
			}
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
								ToFunctionTextLogCallback.Call(_packet.Data(), _packet.DataLen(), _sendNextCommandAsResponse, _responseAddress);
							}
							break;
						case FunctionCodeWirelessModem:
							{
								ToFunctionWirelessModemCallback.Call(_packet.Data(), _packet.DataLen(), _sendNextCommandAsResponse, _responseAddress);
							}
							break;
						case FunctionCodeSeismicSensor:
							{
								ToFunctionSeismicSensorCallback.Call(_packet.Data(), _packet.DataLen(), _sendNextCommandAsResponse, _responseAddress);
							}
							break;
						case FunctionCodeSubscriptionService:
							{
								ToFunctionSubscriptionServiceCallback.Call(_packet.Data(), _packet.DataLen(), _sendNextCommandAsResponse, _responseAddress);
							}
							break;
						case FunctionCodeCamera:
							{
								ToFunctionCameraCallback.Call(_packet.Data(), _packet.DataLen(), _sendNextCommandAsResponse, _responseAddress);
							}
							break;
						case FunctionCodeStorage:
							{
								ToFunctionStorageCallback.Call(_packet.Data(), _packet.DataLen(), _sendNextCommandAsResponse, _responseAddress);
							}
							break;
						case FunctionCodeWirebreakSensor:
							{
								ToFunctionWirebreakSensorCallback.Call(_packet.Data(), _packet.DataLen(), _sendNextCommandAsResponse, _responseAddress);
							}
							break;
						case FunctionCodeRelay:
							{
								ToFunctionRelayCallback.Call(_packet.Data(), _packet.DataLen(), _sendNextCommandAsResponse, _responseAddress);
							}
							break;
						case FunctionCodeRadiodozor:
							{
								ToFunctionRadiodozorCallback.Call(_packet.Data(), _packet.DataLen(), _sendNextCommandAsResponse, _responseAddress);
							}
							break;
						case FunctionCodeDiagnostics:
							{
								ToFunctionDiagnosticsCallback.Call(_packet.Data(), _packet.DataLen(), _sendNextCommandAsResponse, _responseAddress);
							}
							break;
						case FunctionCodeActionService:
							{
								ToFunctionActionServiceCallback.Call(_packet.Data(), _packet.DataLen(), _sendNextCommandAsResponse, _responseAddress);
							}
							break;
						case FunctionCodeAlarmLog:
							{
								ToFunctionAlarmLogCallback.Call(_packet.Data(), _packet.DataLen(), _sendNextCommandAsResponse, _responseAddress);
							}
							break;
						case FunctionCodeClock:
							{
								ToFunctionClockCallback.Call(_packet.Data(), _packet.DataLen(), _sendNextCommandAsResponse, _responseAddress);
							}
							break;
						case FunctionCodePowerMonitor:
							{
								ToFunctionPowerMonitorCallback.Call(_packet.Data(), _packet.DataLen(), _sendNextCommandAsResponse, _responseAddress);
							}
							break;
						case FunctionCodeRadiobeacon:
							{
								ToFunctionRadiobeaconCallback.Call(_packet.Data(), _packet.DataLen(), _sendNextCommandAsResponse, _responseAddress);
							}
							break;
						case FunctionCodeLocation:
							{
								ToFunctionLocationCallback.Call(_packet.Data(), _packet.DataLen(), _sendNextCommandAsResponse, _responseAddress);
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
		ImplementerWireless<maxDataSize> *ImplementerWireless<maxDataSize>::_selfPtr = 0;
}
}
}
}

#endif