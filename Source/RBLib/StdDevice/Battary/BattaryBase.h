///////////////////////////////////////////////////////////////////////////////
//	Базовый класс драйвера батареи питания
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef BATTARYBASE_H
#define BATTARYBASE_H

#include "BattaryType.h"

namespace Rblib
{
namespace StdDevice
{
enum BattaryCommand
{
	BattaryCommandUnknown = 0,
	BattaryCommandRemainingLifeTime,
	BattaryCommandVip1026ConsumerInfo,
	BattaryCommandVip1026Info,
	BattaryCommandVip1026State,
	BattaryCommandVip1026OperatingTime,
	BattaryCommandVip1026ConsumersList,
	BattaryCommandVip1026ResetCounters
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Эти структуры передаются в пакетах FirTwig как есть, не изменяйте их, чтобы не потерять совместимость //
///////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma pack(push, 1)

// Время жизни и остаточный заряд
struct BattaryRemainingLifeTimeInfo
{
	unsigned short Days;	// количество отставшихся дней работы батареи
	unsigned char Charge;	// заряд (сколько осталось), %
	BattaryRemainingLifeTimeInfo()
	{
		Days = 0;
		Charge = 0;
	}
};

// Информация о батарее
struct BattaryInfoStruct
{
	BattaryType Type;				// тип батареи
	unsigned char Serial[8];		// серийный номер
	float Capacity;					// начальная емкость батареи в Ач
	unsigned char SectionsCount;	// количество секций
	unsigned char CurrentSection;	// подключенная секция начиная с нуля
	
	BattaryInfoStruct()
	{
		Type = BattaryTypeUnknown;
		for (char i = 0; i < 8; i++)
			Serial[i] = 0xFF;
		Capacity = 0.0;
		SectionsCount = 0;
		CurrentSection = 0;
	}
};

// Состояние батареи
struct BattaryStateStruct
{
	float ExpendedCapacity;		// израсходованный заряд (Ач)
	float Charge;				// остаточный заряд (%)
	float Temperature;			// температура (градусы Цельсия)
	float Voltage;				// напряжение (В)
	
	BattaryStateStruct()
	{
		ExpendedCapacity = 0.0;
		Charge = 0.0;
		Temperature = 0.0;
		Voltage = 0.0;
	}
};

// Информация о потребителе хранимая в батарейке
struct BattaryConsumerInfoStruct
{
	unsigned char DeviceType;	// Тип РС
	unsigned short Serial;		// Серийный номер (SerialLow)
	char SoftwareVersion[15];	// Версия ПО
	
	BattaryConsumerInfoStruct()
	{
		DeviceType = 0;
		Serial = 0;
		for (char i = 0; i < sizeof(SoftwareVersion); i++)
		{
			SoftwareVersion[i] = 0;
		}
	}
};

// Запись списка потребителей (первая часть)
struct BattaryConsumersListRecordPartOneStruct
{
	unsigned char RecordNumber;		// номер записи
	unsigned char PartNumber;		// номер части записи (№ 0)
	unsigned long ConnectTime;		// время подключения, начиная со старта батарейки (сек.)
	unsigned long DisconnectTime;	// время отключения, начиная со старта батарейки (сек.)
	float ExpendedCapacity;			// израсходованный заряд (Ач)
	float Discharge;				// израсходованный заряд (%)
	
	BattaryConsumersListRecordPartOneStruct()
	{
		RecordNumber = 0;
		PartNumber = 0;
		ConnectTime = 0xFFFFFFFF;
		DisconnectTime = 0xFFFFFFFF;
		ExpendedCapacity = 0.0;
		Discharge = 0.0;
	}
};

// Запись списка потребителей (вторая часть)
struct BattaryConsumersListRecordPartTwoStruct
{
	unsigned char RecordNumber;		// номер записи
	unsigned char PartNumber;		// номер части записи (№ 1)
	BattaryConsumerInfoStruct Info;	// информация о потребителе
	
	BattaryConsumersListRecordPartTwoStruct()
	{
		RecordNumber = 0;
		PartNumber = 1;
	}
};

// Информация о счетчиках перезагрузки батарейки
struct BattaryResetCountersStruct
{
	unsigned short SoftResetCount;	// Счётчик программных перезагрузок
	unsigned short HardResetCount;	// Счётчик аппаратных перезагрузок
	
	BattaryResetCountersStruct()
	{
		SoftResetCount = 0;
		HardResetCount = 0;
	}
};

#pragma pack (pop)

class BattaryRequestBase
{
protected:
	BattaryCommand _cmd;
public:
	BattaryRequestBase() : _cmd(BattaryCommandUnknown)
	{
	
	}
	
	BattaryCommand GetCommand()
	{
		return _cmd;
	}
};

class BattaryRequestRemainingLifeTime : public BattaryRequestBase
{
public:
	BattaryRequestRemainingLifeTime()
	{
		_cmd = BattaryCommandRemainingLifeTime;
	}
};

class BattaryRequestVip1026ConsumerInfo : public BattaryRequestBase
{
public:
	BattaryConsumerInfoStruct ConsumerInfo;
public:
	BattaryRequestVip1026ConsumerInfo()
	{
		_cmd = BattaryCommandVip1026ConsumerInfo;
	}
};

class BattaryRequestVip1026Info : public BattaryRequestBase
{
public:
	BattaryRequestVip1026Info()
	{
		_cmd = BattaryCommandVip1026Info;
	}
};

class BattaryRequestVip1026State : public BattaryRequestBase
{
public:
	BattaryRequestVip1026State()
	{
		_cmd = BattaryCommandVip1026State;
	}
};

class BattaryRequestVip1026ConsumersList : public BattaryRequestBase
{
public:
	unsigned char RecordNumber;	// Номер записи
public:
	BattaryRequestVip1026ConsumersList()
	{
		_cmd = BattaryCommandVip1026ConsumersList;
		RecordNumber = 0;
	}
};

class BattaryRequestVip1026ResetCounters : public BattaryRequestBase
{
public:
	BattaryRequestVip1026ResetCounters()
	{
		_cmd = BattaryCommandVip1026ResetCounters;
	}
};

class BattaryResponseBase
{
protected:
	BattaryCommand _cmd;
public:
	BattaryResponseBase() : _cmd(BattaryCommandUnknown)
	{
	
	}
	
	BattaryCommand GetCommand()
	{
		return _cmd;
	}
};

class BattaryResponseRemainingLifeTime : public BattaryResponseBase
{
public:
	BattaryRemainingLifeTimeInfo Info;
public:
	BattaryResponseRemainingLifeTime()
	{
		_cmd = BattaryCommandRemainingLifeTime;
	}
};

class BattaryResponseVip1026ConsumerInfo : public BattaryResponseBase
{
public:
	bool Ack;	// подтверждение приема данных о потребителе
public:
	BattaryResponseVip1026ConsumerInfo()
	{
		_cmd = BattaryCommandVip1026ConsumerInfo;
		Ack = 0;
	}
};

class BattaryResponseVip1026Info : public BattaryResponseBase
{
public:
	BattaryInfoStruct Info;	// информация о батарее
public:
	BattaryResponseVip1026Info()
	{
		_cmd = BattaryCommandVip1026Info;
	}
};

class BattaryResponseVip1026State : public BattaryResponseBase
{
public:
	unsigned long UpTime;		// время  прошедшее с момента запуска (сек.)
	BattaryStateStruct State;	// состояние батареи
public:
	BattaryResponseVip1026State()
	{
		_cmd = BattaryCommandVip1026State;
		UpTime = 0;
	}
};

class BattaryResponseVip1026ConsumersList : public BattaryResponseBase
{
public:
	unsigned char RecordsCount;							// сколько всего записей в списке
	BattaryConsumersListRecordPartOneStruct PartOne;	// первая часть записи
	BattaryConsumersListRecordPartTwoStruct PartTwo;	// вторая часть записи
public:
	BattaryResponseVip1026ConsumersList()
	{
		_cmd = BattaryCommandVip1026ConsumersList;
		RecordsCount = 0;
	}
};

class BattaryResponseVip1026ResetCounters : public BattaryResponseBase
{
public:
	BattaryResetCountersStruct ResetCounters;	// информация о счетчиках перезагрузок
public:
	BattaryResponseVip1026ResetCounters()
	{
		_cmd = BattaryCommandVip1026ResetCounters;
	}
};

/// Базовый класс различных типов батарей
class BattaryBase
{
public:
	// Тип функции обратного вызова при получении ответа от батарейки
	typedef void (OnResponse)(bool result, BattaryResponseBase * response);
public:
	/// Конструктор по умолчанию
	BattaryBase()
	{

	}
	
	/// Деструткор
	virtual ~BattaryBase()
	{

	}
	
	/// Тип батареи
	virtual BattaryType GetType()
	{
		return BattaryTypeUnknown;
	}
	
	virtual void Wait()
	{
	
	}
	
	virtual bool Busy()
	{
		return false;
	}
	
	/// Выполнить запрос
	virtual void Request(BattaryRequestBase *request, OnResponse *onResponse)
	{
		if (request)
		{
			if (request->GetCommand() == BattaryCommandRemainingLifeTime)
			{
				if (onResponse)
				{
					BattaryResponseRemainingLifeTime response;
					onResponse(true, &response);
					return;
				}
			}
		}
		
		if (onResponse)
			onResponse(false, 0);
	}
};

}
}

#endif