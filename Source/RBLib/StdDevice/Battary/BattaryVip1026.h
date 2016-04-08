///////////////////////////////////////////////////////////////////////////////
//	Класс драйвера батареи ВИП-1026
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef BATTARYVIP1026_H
#define BATTARYVIP1026_H

#include "BattaryBase.h"

namespace Rblib
{
namespace StdDevice
{

/// Драйвер батареи ВИП-1026
template<class i2cBus>
class BattaryVip1026 : public BattaryBase
{
protected:
	static const unsigned char _slaveAddress = 0x77;
	static const unsigned char _bufferSize = 128;
	static const unsigned short _crc16Table[256];
	
	enum Command
	{
		CmdUnknown = 0,
		CmdSetConsumerInfo = 0x20,
		CmdGetRemainingLifeTime = 0x21,
		CmdGetState = 0x22,
		CmdGetConsumersList = 0x23,
		CmdGetInfo = 0x24,
		CmdGetResetCounters = 0x25,
		CmdGetStateVer2 = 0x28			// то же, что CmdGetState, только напряжение 4-х байтное
	};
	
	enum State
	{
		StateIdle = 0,
		StateWaitWrite,
		StateWrited,
		StateWaitRead,
		StateReaded
	};
	
	static volatile State _state;
	static bool _lastI2CResult;
	OnResponse *_onResponse;
	Command _lastCommand;
	unsigned char _responseSize;
	unsigned char _buffer[_bufferSize];
	unsigned short _realType;	// реальный тип батареи, который сообщила сама батарея
public:
	/// Конструктор по умолчанию
	BattaryVip1026()
	{
		_state = StateIdle;
		_responseSize = 0;
		_lastCommand = CmdUnknown;
		_realType = BattaryTypeVip1026;
	}

	/// Деструткор
	virtual ~BattaryVip1026()
	{

	}
	
	/// Тип батареи
	virtual BattaryType GetType()
	{
		return BattaryTypeVip1026;
	}

	/// Тип t обслуживается данным классом
	virtual bool TypeEqual(unsigned short t)
	{
		// пока всё подряд обслуживается этим классом
		return true;
	}

	/// Реальный тип батареи
	virtual unsigned short GetRealType()
	{
		return _realType;
	}
	
	virtual void Wait()
	{
		switch(_state)
		{
		case StateWaitWrite:
			{
				if (_onResponse)
					_onResponse(false, 0);
				EndRequest();
			}
			break;
		case StateWrited:
			{
				if (_lastI2CResult)
				{
					if (_responseSize > 0)
					{
						GetResponse(_responseSize);
					}
					else
					{
						EndRequest();
					}
				}
				else
				{
					if (_onResponse)
						_onResponse(_lastI2CResult, 0);
					EndRequest();
				}
			}
			break;
		case StateWaitRead:
			{
				if (_onResponse)
					_onResponse(false, 0);
				EndRequest();
			}
			break;
		case StateReaded:
			{
				CheckResponse(_lastI2CResult);
				EndRequest();
			}
			break;
		}
	}
	
	virtual bool Busy()
	{
		return _state != StateIdle;
	}
	
	/// Выполнить запрос
	virtual void Request(BattaryRequestBase *request, OnResponse *onResponse)
	{
		// занято
		if (Busy())
			return;
		if (0 == request)
			return;
		// некуда будет отдать ответ, поэтому не стоит отравлять запрос
		if (0 == onResponse)
			return;
		// куда отдать ответ
		_onResponse = onResponse;
		
		bool res = false;
		Command cmd = CmdUnknown;
		unsigned char *cmdData = 0;
		unsigned char cmdDataLen = 0;
		
		switch(request->GetCommand())
		{
		case BattaryCommandRemainingLifeTime:
			{
				cmd = CmdGetRemainingLifeTime;
				_responseSize = 6;
			}
			break;
		case BattaryCommandVip1026ConsumerInfo:
			{
				BattaryRequestVip1026ConsumerInfo *requestPrt = (BattaryRequestVip1026ConsumerInfo *)request;
				
				unsigned char deviceInformation[32];
				memset(deviceInformation, 0, sizeof(deviceInformation));
				unsigned char copy = sizeof(deviceInformation);
				if (sizeof(requestPrt->ConsumerInfo) < copy)
					copy = sizeof(requestPrt->ConsumerInfo);
				memcpy(deviceInformation, &requestPrt->ConsumerInfo, copy);
				
				cmd = CmdSetConsumerInfo;
				cmdData = deviceInformation;
				cmdDataLen = sizeof(deviceInformation);
				_responseSize = 3;
			}
			break;
		case BattaryCommandVip1026Info:
			{
				cmd = CmdGetInfo;
				_responseSize = 16;
			}
			break;
		case BattaryCommandVip1026State:
			{
				cmd = CmdGetStateVer2;
				_responseSize = 18;
			}
			break;
		case BattaryCommandVip1026ConsumersList:
			{
				BattaryRequestVip1026ConsumersList *requestPrt = (BattaryRequestVip1026ConsumersList *)request;
				
				cmd = CmdGetConsumersList;
				cmdData = &requestPrt->RecordNumber;
				cmdDataLen = sizeof(requestPrt->RecordNumber);
				_responseSize = 49;
			}
			break;
		case BattaryCommandVip1026ResetCounters:
			{
				cmd = CmdGetResetCounters;
				_responseSize = 6;
			}
			break;
		};
		
		if (cmd != CmdUnknown)
		{
			// Отправляем запрос
			res = SendRequest(cmd, cmdData, cmdDataLen);
		}
		if (!res)
			onResponse(false, 0);
	}
protected:
	void EndRequest()
	{
		i2cBus::Break();
		_state = StateIdle;
	}
	
	bool SendRequest(Command cmd, unsigned char *data, unsigned char dataLen)
	{
		if (dataLen > 0 && 0 == data)
			return false;
		
		// обрезаем данные, если их слишком много
		if (dataLen > _bufferSize - 1 - 2)
			dataLen = _bufferSize - 1 - 2;
		
		unsigned char totalLen = 0;
		unsigned short crc = 0xFFFF;
		// заполняем буфер параллельно вычисляя CRC
		_buffer[totalLen] = cmd;
		crc = Crc16(_buffer[totalLen], crc);
		totalLen++;
		for (unsigned char i = 0; i < dataLen; i++)
		{
			_buffer[totalLen] = data[i];
			crc = Crc16(_buffer[totalLen], crc);
			totalLen++;
		}
		_buffer[totalLen] = crc & 0xFF;
		_buffer[totalLen + 1] = (crc >> 8) & 0xFF;
		totalLen += 2;
		// запомним последнюю команду
		_lastCommand = cmd;
		// будем ждать завершения записи
		_state = StateWaitWrite;
		// пишем в шину асинхронно
		i2cBus::Break();
		i2cBus::SetSlaveAddress(_slaveAddress);
		bool res = i2cBus::Write(_buffer, totalLen, I2CWriteCallback);
		// если не удалось начать запись, завершаем обработку запроса
		if (!res)
			EndRequest();
		return res;
	}
	
	bool GetResponse(unsigned int len)
	{
		// будем ждать завершения чтения
		_state = StateWaitRead;
		// читаем шину асихронно
		bool res = i2cBus::Read(_buffer, len, I2CReadCallback);
		// если не удалось начать чтение, завершаем обработку запроса
		if (!res)
			EndRequest();
		return res;
	}
	
	void CheckResponse(bool readResult)
	{
		unsigned char responseLen = _responseSize;
		unsigned short crc = 0xFFFF;
		for (unsigned char i = 0; i < responseLen - 2; i++)
		{
			crc = Crc16(_buffer[i], crc);
		}
		unsigned short responseCrc;
		responseCrc = _buffer[responseLen - 1];
		responseCrc <<= 8;
		responseCrc |= _buffer[responseLen - 2];
		bool crcOk = crc == responseCrc;
		
		if (_onResponse)
		{
			if (readResult && crcOk)
			{
				switch(_lastCommand)
				{
				case CmdGetRemainingLifeTime:
					{
						BattaryResponseRemainingLifeTime response;
						response.Info.Days = _buffer[1];
						response.Info.Days <<= 8;
						response.Info.Days |= _buffer[0];
						
						float charge = _buffer[3];
						charge += (float)(_buffer[2] / (float)0x100);
						
						response.Info.Charge = (unsigned char)charge;

						_onResponse(readResult, &response);
					}
					break;
				case CmdSetConsumerInfo:
					{
						BattaryResponseVip1026ConsumerInfo response;
						response.Ack = _buffer[0];
						_onResponse(readResult, &response);
					}
					break;
				case CmdGetInfo:
					{
						BattaryResponseVip1026Info response;
						unsigned short type = _buffer[1];
						type <<= 8;
						type |= _buffer[0];

						// запоминаем реальный тип батареи
						_realType = type;

						response.Info.Type = (BattaryType)type;
						for (char i = 0; i < 8; i++)
							response.Info.Serial[i] = _buffer[2 + i];
						response.Info.Capacity = _buffer[11];
						response.Info.Capacity += (float)(_buffer[10] / (float)0x100);
						response.Info.SectionsCount = _buffer[12];
						response.Info.CurrentSection = _buffer[13];
						_onResponse(readResult, &response);
					}
					break;
				case CmdGetState:
					{
						BattaryResponseVip1026State response;
						unsigned long upTime = _buffer[3];
						upTime <<= 8;
						upTime |= _buffer[2];
						upTime <<= 8;
						upTime |= _buffer[1];
						upTime <<= 8;
						upTime |= _buffer[0];
						response.UpTime = upTime;
						response.State.ExpendedCapacity = _buffer[7];
						unsigned long ExpendedCapacityFraction = _buffer[6];
						ExpendedCapacityFraction <<= 8;
						ExpendedCapacityFraction |= _buffer[5];
						ExpendedCapacityFraction <<= 8;
						ExpendedCapacityFraction |= _buffer[4];
						response.State.ExpendedCapacity += (float)(ExpendedCapacityFraction / (float)0x1000000);
						response.State.Charge = _buffer[9];
						response.State.Charge += (float)(_buffer[8] / (float)0x100);
						response.State.Temperature = _buffer[11];
						response.State.Temperature += (float)(_buffer[10] / (float)0x100);
						response.State.Voltage = (_buffer[12] >> 4) & 0x0F;
						response.State.Voltage += (float)((_buffer[12] & 0x0F) / (float)0x10);
						_onResponse(readResult, &response);
					}
					break;
				case CmdGetConsumersList:
					{
						BattaryResponseVip1026ConsumersList response;
						
						unsigned long connectTime = _buffer[3];
						connectTime <<= 8;
						connectTime |= _buffer[2];
						connectTime <<= 8;
						connectTime |= _buffer[1];
						connectTime <<= 8;
						connectTime |= _buffer[0];
						
						unsigned long disconnectTime = _buffer[7];
						disconnectTime <<= 8;
						disconnectTime |= _buffer[6];
						disconnectTime <<= 8;
						disconnectTime |= _buffer[5];
						disconnectTime <<= 8;
						disconnectTime |= _buffer[4];
						
						unsigned char expendedCapacity = _buffer[11];
						unsigned long expendedCapacityFraction = _buffer[10];
						expendedCapacityFraction <<= 8;
						expendedCapacityFraction |= _buffer[9];
						expendedCapacityFraction <<= 8;
						expendedCapacityFraction |= _buffer[8];
						
						float discharge = _buffer[13];
						discharge += (float)(_buffer[12] / (float)0x100);
						
						unsigned char deviceInfo[32];
						for (int i = 0; i < 32; i++)
						{
							deviceInfo[i] = _buffer[14 + i];
						}
						
						unsigned char recordsCount = _buffer[46];
						
						response.RecordsCount = recordsCount;
						response.PartOne.ConnectTime = connectTime;
						response.PartOne.DisconnectTime = disconnectTime;
						response.PartOne.ExpendedCapacity = expendedCapacity;
						response.PartOne.ExpendedCapacity += (float)(expendedCapacityFraction / (float)0x1000000);
						response.PartOne.Discharge = discharge;
						
						unsigned char devInfoLen =  sizeof(response.PartTwo.Info);
						memcpy(&response.PartTwo.Info, deviceInfo, devInfoLen);
						
						_onResponse(readResult, &response);
					}
					break;
				case CmdGetResetCounters:
					{
						BattaryResponseVip1026ResetCounters response;
						response.ResetCounters.SoftResetCount = _buffer[1];
						response.ResetCounters.SoftResetCount <<= 8;
						response.ResetCounters.SoftResetCount |= _buffer[0];
						response.ResetCounters.HardResetCount = _buffer[3];
						response.ResetCounters.HardResetCount <<= 8;
						response.ResetCounters.HardResetCount |= _buffer[2];
						_onResponse(readResult, &response);
					}
					break;
				case CmdGetStateVer2:
					{
						BattaryResponseVip1026State response;
						unsigned long upTime = _buffer[3];
						upTime <<= 8;
						upTime |= _buffer[2];
						upTime <<= 8;
						upTime |= _buffer[1];
						upTime <<= 8;
						upTime |= _buffer[0];
						response.UpTime = upTime;
						response.State.ExpendedCapacity = _buffer[7];
						unsigned long ExpendedCapacityFraction = _buffer[6];
						ExpendedCapacityFraction <<= 8;
						ExpendedCapacityFraction |= _buffer[5];
						ExpendedCapacityFraction <<= 8;
						ExpendedCapacityFraction |= _buffer[4];
						response.State.ExpendedCapacity += (float)(ExpendedCapacityFraction / (float)0x1000000);
						response.State.Charge = _buffer[9];
						response.State.Charge += (float)(_buffer[8] / (float)0x100);
						response.State.Temperature = _buffer[11];
						response.State.Temperature += (float)(_buffer[10] / (float)0x100);
						response.State.Voltage = _buffer[15];
						unsigned long VoltageFraction = _buffer[14];
						VoltageFraction <<= 8;
						VoltageFraction |= _buffer[13];
						VoltageFraction <<= 8;
						VoltageFraction |= _buffer[12];
						response.State.Voltage += (float)(VoltageFraction / (float)0x1000000);
						_onResponse(readResult, &response);
					}
					break;
				}
			}
			else
			{
				_onResponse(readResult, 0);
			}
		}
		
		_responseSize = 0;
		_lastCommand = CmdUnknown;
	}
	
	static void I2CWriteCallback(bool result)
	{
		_lastI2CResult = result;
		if (StateWaitWrite == _state)
			_state = StateWrited;
	}
	
	static void I2CReadCallback(bool result)
	{
		_lastI2CResult = result;
		if (StateWaitRead == _state)
			_state = StateReaded;
	}
	
	inline static unsigned short Crc16(unsigned char newByte, unsigned short previousCrcValue)
	{
		return _crc16Table[(previousCrcValue >> 8) ^ newByte] ^ (previousCrcValue << 8);
	}
};

// инстанцирование статических переменных
template<class i2cBus>
volatile typename BattaryVip1026<i2cBus>::State BattaryVip1026<i2cBus>::_state;

template<class i2cBus>
bool BattaryVip1026<i2cBus>::_lastI2CResult;

template<class i2cBus>
const unsigned short BattaryVip1026<i2cBus>::_crc16Table[256] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
	0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
	0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
	0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
	0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
	0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
	0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
	0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
	0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
	0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

}
}

#endif