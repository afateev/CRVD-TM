///////////////////////////////////////////////////////////////////////////////
//	Класс доступа к конфигурации узла
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MODEM_NODECONFIG_H
#define MODEM_NODECONFIG_H

#include "NodeConfigStruct.h"
#include "../../Utils/CallbackWrapper.h"

namespace Rblib
{
namespace Modem
{
namespace NodeConfig
{
	class NodeConfig
	{
	public:
		typedef NodeConfigStruct NodeConfigStruct;
		
		// параметры конфигурации сети
		enum NetParam
		{
			NetId								= 0,
			NetFrequencyChannel					= 1,
			NetMainTimeSlotCount				= 2,
			NetAdditionalTimeSlotCount			= 3,
			NetSyncPeriod						= 4,
			NetSyncTimeout						= 5,
			NetActivityTableDistributionPeriod	= 6,
			NetAdditionalTimeSlotUsedCount		= 7,
			NetAdditionalTimeSlotStart			= 8,
			NetRelayMode						= 9,
			NetPacketRepeatCount				= 10,
			NetRadioListetingInterval			= 11,
		};
		
		// параметры узла сети
		enum NodeParam
		{
			NodeId				= 0,
			NodeSerialNumber	= 1
		};
		
		typedef CallbackWrapper<void *, unsigned int> ReadCallbackType;
		typedef CallbackWrapper<void *, unsigned int> WriteCallbackType;
	public:
		ReadCallbackType ReadCallback;
		WriteCallbackType WriteCallback;
	protected:
		NodeConfigStruct _currentConfig;
		NodeConfigStruct _newConfig;
	public:
		// Конструктор по умолчанию
		NodeConfig()
		{
			ResetToDefault();
		}
		
		// Установить значения по умолчанию
		void ResetToDefault()
		{
			_currentConfig = NodeConfigStruct();
		}
		
		// Получить конфигурацию узла в виде структуры
		NodeConfigStruct GetCurrentConfig()
		{
			NodeConfigStruct res;
			memcpy(&res, &_currentConfig, sizeof(res));
			return res;
		}
		
		static void GetCurrentConfig(void *callbackParam, NodeConfigStruct &nodeConfigStruct)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			NodeConfig *im = (NodeConfig *)callbackParam;
			
			nodeConfigStruct = im->GetCurrentConfig();
		}
		
		// Получить определенный параметр конфигурации сети
		int GetNetParam(NetParam netParam)
		{
			int res = 0;
			
			switch(netParam)
			{
			case NetId:
				res = _currentConfig.NetConfig.Id;
				break;
			case NetFrequencyChannel:
				res = _currentConfig.NetConfig.FrequencyChannel;
				break;
			case NetMainTimeSlotCount:
				res = _currentConfig.NetConfig.MainTimeSlotCount;
				break;
			case NetAdditionalTimeSlotCount:
				res = _currentConfig.NetConfig.AdditionalTimeSlotCount;
				break;
			case NetSyncPeriod:
				res = _currentConfig.NetConfig.SyncPeriod;
				break;
			case NetSyncTimeout:
				res = _currentConfig.NetConfig.SyncTimeout;
				break;
			case NetActivityTableDistributionPeriod:
				res = _currentConfig.NetConfig.ActivityTableDistributionPeriod;
				break;
			case NetAdditionalTimeSlotUsedCount:
				res = _currentConfig.NetConfig.AdditionalTimeSlotUsedCount;
				break;
			case NetAdditionalTimeSlotStart:
				res = _currentConfig.NetConfig.AdditionalTimeSlotStart;
				break;
			case NetRelayMode:
				res = _currentConfig.NetConfig.RelayMode;
				break;
			case NetPacketRepeatCount:
				res = _currentConfig.NetConfig.PacketRepeatCount;
				break;
			case NetRadioListetingInterval:
				res = _currentConfig.NetConfig.RadioListetingInterval;
				break;
			}
			
			return res;
		}
		
		static void GetConfigParams(void *callbackParam, int requestedParamsCount, Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCode *requestedParams,
							   Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCode *returnedIdsBuffer, Rblib::Protocol::G1::Function::WirelessModem::ConfigParamValueType *returnedValuesBuffer, int maxItemsCount, int &returnedParamsCount)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			NodeConfig *im = (NodeConfig *)callbackParam;
			
			bool returnAll = false;
			Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCode *ids = requestedParams;
			// если запрашивается 0 параметров, значит надо выдать все
			if (requestedParamsCount == 0)
			{
				returnAll = true;
				// это типа размер полного списка параметров
				requestedParamsCount = 14;
				// под список запрошенных параметров используем список возврящаемых параметров, т.к. его размер известен, а размер и существование списка requestedParams не понятно
				ids = returnedIdsBuffer;
			}
			
			// ограничение длины списка
			returnedParamsCount = requestedParamsCount;
			if (returnedParamsCount > maxItemsCount)
			{
				returnedParamsCount = maxItemsCount;
			}
			
			if (returnAll)
			{
				// заполняем список "как бы запрошенных" параметров
				for (int i = 0; i < returnedParamsCount; i++)
				{
					switch (i)
					{
					case 0:
						ids[i] = Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetId;
						break;
					case 1:
						ids[i] = Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetFrequencyChannel;
						break;
					case 2:
						ids[i] = Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetMainTimeSlotCount;
						break;
					case 3:
						ids[i] = Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetAdditionalTimeSlotCount;
						break;
					case 4:
						ids[i] = Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetSyncPeriod;
						break;
					case 5:
						ids[i] = Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetSyncTimeout;
						break;
					case 6:
						ids[i] = Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetActivityTableDistributionPeriod;
						break;
					case 7:
						ids[i] = Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNodeId;
						break;
					case 8:
						ids[i] = Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNodeSerialNumber;
						break;
					case 9:
						ids[i] = Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetAdditionalTimeSlotUsedCount;
						break;
					case 10:
						ids[i] = Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetAdditionalTimeSlotStart;
						break;
					case 11:
						ids[i] = Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetRelayMode;
						break;
					case 12:
						ids[i] = Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetPacketRepeatCount;
						break;
					case 13:
						ids[i] = Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetRadioListetingInterval;
						break;
					/* на будущее
					case 12:
						ids[i] = ...;
						break;
					*/
					}
				}
			}
			
			for (int i = 0; i < returnedParamsCount; i++)
			{
				Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCode paramCode = (Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCode)ids[i];
				Rblib::Protocol::G1::Function::WirelessModem::ConfigParamValueType value = 0;
				
				switch(paramCode)
				{
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetId:
					{
						value = im->GetNetParam(NetId);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetFrequencyChannel:
					{
						value = im->GetNetParam(NetFrequencyChannel);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetMainTimeSlotCount:
					{
						value = im->GetNetParam(NetMainTimeSlotCount);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetAdditionalTimeSlotCount:
					{
						value = im->GetNetParam(NetAdditionalTimeSlotCount);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetSyncPeriod:
					{
						value = im->GetNetParam(NetSyncPeriod);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetSyncTimeout:
					{
						value = im->GetNetParam(NetSyncTimeout);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetActivityTableDistributionPeriod:
					{
						value = im->GetNetParam(NetActivityTableDistributionPeriod);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNodeId:
					{
						value = im->GetNodeParam(NodeId);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNodeSerialNumber:
					{
						value = im->GetNodeParam(NodeSerialNumber);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetAdditionalTimeSlotUsedCount:
					{
						value = im->GetNetParam(NetAdditionalTimeSlotUsedCount);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetAdditionalTimeSlotStart:
					{
						value = im->GetNetParam(NetAdditionalTimeSlotStart);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetRelayMode:
					{
						value = im->GetNetParam(NetRelayMode);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetPacketRepeatCount:
					{
						value = im->GetNetParam(NetPacketRepeatCount);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetRadioListetingInterval:
					{
						value = im->GetNetParam(NetRadioListetingInterval);
					}
					break;
				}
				returnedIdsBuffer[i] = paramCode;
				returnedValuesBuffer[i] = value;
			}
		}
		
		// Установить определенный параметр конфигурации сети
		bool SetNetParam(NetParam netParam, int value)
		{
			bool res = true;
			
			switch(netParam)
			{
			case NetId:
				_newConfig.NetConfig.Id = value;
				break;
			case NetFrequencyChannel:
				_newConfig.NetConfig.FrequencyChannel = value;
				break;
			case NetMainTimeSlotCount:
				_newConfig.NetConfig.MainTimeSlotCount = value;
				break;
			case NetAdditionalTimeSlotCount:
				_newConfig.NetConfig.AdditionalTimeSlotCount = value;
				break;
			case NetSyncPeriod:
				_newConfig.NetConfig.SyncPeriod = value;
				break;
			case NetSyncTimeout:
				_newConfig.NetConfig.SyncTimeout = value;
				break;
			case NetActivityTableDistributionPeriod:
				_newConfig.NetConfig.ActivityTableDistributionPeriod = value;
				break;
			case NetAdditionalTimeSlotUsedCount:
				_newConfig.NetConfig.AdditionalTimeSlotUsedCount = value;
				break;
			case NetAdditionalTimeSlotStart:
				_newConfig.NetConfig.AdditionalTimeSlotStart = value;
				break;
			case NetRelayMode:
				_newConfig.NetConfig.RelayMode = value;
				break;
			case NetPacketRepeatCount:
				_newConfig.NetConfig.PacketRepeatCount = value;
				break;
			case NetRadioListetingInterval:
				_newConfig.NetConfig.RadioListetingInterval = value;
				break;
			default:
				res = false;
			}
			
			if (res)
			{
				res = Save();
			}
			
			return res;
		}
		
		// Получить определенный параметр конфигурации узла
		int GetNodeParam(NodeParam nodeParam)
		{
			int res = 0;
			
			switch(nodeParam)
			{
			case NodeId:
				res = _currentConfig.Id;
				break;
			case NodeSerialNumber:
				res = _currentConfig.SerialNumber;
				break;
			}
			
			return res;
		}
		
		// Установить определенный параметр конфигурации узла
		bool SetNodeParam(NodeParam nodeParam, int value)
		{
			bool res = true;
			
			switch(nodeParam)
			{
			case NodeId:
				_newConfig.Id = value;
				break;
			case NodeSerialNumber:
				_newConfig.SerialNumber = value;
				break;
			default:
				res = false;
			}
			
			if (res)
			{
				res = Save();
			}
			
			return res;
		}
		
		static void SetConfigParams(void *callbackParam, int paramsCount, Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCode *params,
					  Rblib::Protocol::G1::Function::WirelessModem::ConfigParamValueType *values, bool *results, int maxItemsCount, int &returnedItemsCount)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			NodeConfig *im = (NodeConfig *)callbackParam;
			
			returnedItemsCount = paramsCount;
			if (returnedItemsCount > maxItemsCount)
			{
				returnedItemsCount = maxItemsCount;
			}
			
			for (int i = 0; i < returnedItemsCount; i++)
			{
				Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCode paramCode = (Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCode)params[i];
				Rblib::Protocol::G1::Function::WirelessModem::ConfigParamValueType value = values[i];
				bool result = false;
				
				switch(paramCode)
				{
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetId:
					{
						result = im->SetNetParam(NetId, value);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetFrequencyChannel:
					{
						result = im->SetNetParam(NetFrequencyChannel, value);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetMainTimeSlotCount:
					{
						result = im->SetNetParam(NetMainTimeSlotCount, value);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetAdditionalTimeSlotCount:
					{
						result = im->SetNetParam(NetAdditionalTimeSlotCount, value);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetSyncPeriod:
					{
						result = im->SetNetParam(NetSyncPeriod, value);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetSyncTimeout:
					{
						result = im->SetNetParam(NetSyncTimeout, value);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetActivityTableDistributionPeriod:
					{
						result = im->SetNetParam(NetActivityTableDistributionPeriod, value);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNodeId:
					{
						result = im->SetNodeParam(NodeId, value);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNodeSerialNumber:
					{
						result = im->SetNodeParam(NodeSerialNumber, value);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetAdditionalTimeSlotUsedCount:
					{
						result = im->SetNetParam(NetAdditionalTimeSlotUsedCount, value);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetAdditionalTimeSlotStart:
					{
						result = im->SetNetParam(NetAdditionalTimeSlotStart, value);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetRelayMode:
					{
						result = im->SetNetParam(NetRelayMode, value);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetPacketRepeatCount:
					{
						result = im->SetNetParam(NetPacketRepeatCount, value);
					}
					break;
				case Rblib::Protocol::G1::Function::WirelessModem::ConfigParamCodeNetRadioListetingInterval:
					{
						result = im->SetNetParam(NetRadioListetingInterval, value);
					}
					break;
				}
				results[i] = result;
			}
		}
		
		// Загрузить сохраненную конфигурацию
		bool Load()
		{
			ReadCallback.Call(&_currentConfig, sizeof(_currentConfig));
			
			bool res = false;
			
			// версии не совпадают
			if (!_currentConfig.VersionValid())
			{
				// обнаружены различия в версиях структур
				switch (_currentConfig.StructureVersion)
				{
				case 1:
					{
						// Другая версия конфигурации
						NodeConfigStructVer1 _configOtherVersion; 
						ReadCallback.Call(&_configOtherVersion, sizeof(_configOtherVersion));
						if (_configOtherVersion.VersionValid())
						{
							if (_configOtherVersion.CrcValid())
							{
								_currentConfig.Reset();
								// Конвертируем _configOtherVersion в _currentConfig
								_currentConfig.NetConfig.Id = _configOtherVersion.NetConfig.Id;
								_currentConfig.NetConfig.FrequencyChannel = _configOtherVersion.NetConfig.FrequencyChannel;
								_currentConfig.NetConfig.MainTimeSlotCount = _configOtherVersion.NetConfig.MainTimeSlotCount;
								_currentConfig.NetConfig.AdditionalTimeSlotCount = _configOtherVersion.NetConfig.AdditionalTimeSlotCount;
								_currentConfig.NetConfig.SyncPeriod = _configOtherVersion.NetConfig.SyncPeriod;
								_currentConfig.NetConfig.SyncTimeout = _configOtherVersion.NetConfig.SyncTimeout;
								_currentConfig.NetConfig.ActivityTableDistributionPeriod = _configOtherVersion.NetConfig.ActivityTableDistributionPeriod;
								_currentConfig.Id = _configOtherVersion.Id;
								_currentConfig.SerialNumber = _configOtherVersion.SerialNumber;
								
								// сохранется _newConfig, синхронизируемся
								_newConfig = _currentConfig;
								
								res = Save();
							}
						}
					}
					break;
				case 2:
					{
						// Другая версия конфигурации
						NodeConfigStructVer2 _configOtherVersion; 
						ReadCallback.Call(&_configOtherVersion, sizeof(_configOtherVersion));
						if (_configOtherVersion.VersionValid())
						{
							if (_configOtherVersion.CrcValid())
							{
								_currentConfig.Reset();
								// Конвертируем _configOtherVersion в _currentConfig
								_currentConfig.NetConfig.Id = _configOtherVersion.NetConfig.Id;
								_currentConfig.NetConfig.FrequencyChannel = _configOtherVersion.NetConfig.FrequencyChannel;
								_currentConfig.NetConfig.MainTimeSlotCount = _configOtherVersion.NetConfig.MainTimeSlotCount;
								_currentConfig.NetConfig.AdditionalTimeSlotCount = _configOtherVersion.NetConfig.AdditionalTimeSlotCount;
								_currentConfig.NetConfig.SyncPeriod = _configOtherVersion.NetConfig.SyncPeriod;
								_currentConfig.NetConfig.SyncTimeout = _configOtherVersion.NetConfig.SyncTimeout;
								_currentConfig.NetConfig.ActivityTableDistributionPeriod = _configOtherVersion.NetConfig.ActivityTableDistributionPeriod;
								_currentConfig.NetConfig.AdditionalTimeSlotUsedCount = _configOtherVersion.NetConfig.AdditionalTimeSlotUsedCount;
								_currentConfig.NetConfig.AdditionalTimeSlotStart = _configOtherVersion.NetConfig.AdditionalTimeSlotStart;
								_currentConfig.Id = _configOtherVersion.Id;
								_currentConfig.SerialNumber = _configOtherVersion.SerialNumber;
								
								// сохранется _newConfig, синхронизируемся
								_newConfig = _currentConfig;
								
								res = Save();
							}
						}
					}
					break;
				case 3:
					{
						// Другая версия конфигурации
						NodeConfigStructVer3 _configOtherVersion; 
						ReadCallback.Call(&_configOtherVersion, sizeof(_configOtherVersion));
						if (_configOtherVersion.VersionValid())
						{
							if (_configOtherVersion.CrcValid())
							{
								_currentConfig.Reset();
								// Конвертируем _configOtherVersion в _currentConfig
								_currentConfig.NetConfig.Id = _configOtherVersion.NetConfig.Id;
								_currentConfig.NetConfig.FrequencyChannel = _configOtherVersion.NetConfig.FrequencyChannel;
								_currentConfig.NetConfig.MainTimeSlotCount = _configOtherVersion.NetConfig.MainTimeSlotCount;
								_currentConfig.NetConfig.AdditionalTimeSlotCount = _configOtherVersion.NetConfig.AdditionalTimeSlotCount;
								_currentConfig.NetConfig.SyncPeriod = _configOtherVersion.NetConfig.SyncPeriod;
								_currentConfig.NetConfig.SyncTimeout = _configOtherVersion.NetConfig.SyncTimeout;
								_currentConfig.NetConfig.ActivityTableDistributionPeriod = _configOtherVersion.NetConfig.ActivityTableDistributionPeriod;
								_currentConfig.NetConfig.AdditionalTimeSlotUsedCount = _configOtherVersion.NetConfig.AdditionalTimeSlotUsedCount;
								_currentConfig.NetConfig.AdditionalTimeSlotStart = _configOtherVersion.NetConfig.AdditionalTimeSlotStart;
								_currentConfig.NetConfig.RelayMode = _configOtherVersion.NetConfig.RelayMode;
								_currentConfig.Id = _configOtherVersion.Id;
								_currentConfig.SerialNumber = _configOtherVersion.SerialNumber;
								
								// сохранется _newConfig, синхронизируемся
								_newConfig = _currentConfig;
								
								res = Save();
							}
						}
					}
					break;
				}
			}
			else
			{
				// версии совпадают
				res = _currentConfig.CrcValid();
			}
			
			// не удалось загрузить или сконвертировать
			if (!res)
			{
				_newConfig.Reset();
				/*
				_newConfig.NetConfig.Id = 0x12345678;
				_newConfig.NetConfig.FrequencyChannel = 0;
				_newConfig.NetConfig.MainTimeSlotCount = 20;
				_newConfig.NetConfig.AdditionalTimeSlotCount = 10;
				_newConfig.NetConfig.SyncPeriod = 1 * 16;
				_newConfig.NetConfig.SyncTimeout = 5;
				_newConfig.NetConfig.ActivityTableDistributionPeriod = 2;
				_newConfig.Id = 2;
				_newConfig.SerialNumber = 2;
				*/
				_currentConfig = _newConfig;
				res = Save();
			}
			
			// копия текущего набора для редактирования
			_newConfig = _currentConfig;
			
			return res;
		}
		
		static void Load(void *callbackParam)
		{
			if (0 == callbackParam)
			{
				return;
			}
			
			NodeConfig *im = (NodeConfig *)callbackParam;
			
			im->Load();
		}
		
		//Сохранить конфигурацию в энергонезависимую память
		bool Save()
		{
			_newConfig.UpdateCrc();
			WriteCallback.Call(&_newConfig, sizeof(_newConfig));
			Load();
			return true;
		}
	};
}
}
}

#endif