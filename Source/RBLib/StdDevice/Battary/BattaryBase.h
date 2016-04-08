///////////////////////////////////////////////////////////////////////////////
//	������� ����� �������� ������� �������
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
// ��� ��������� ���������� � ������� FirTwig ��� ����, �� ��������� ��, ����� �� �������� ������������� //
///////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma pack(push, 1)

// ����� ����� � ���������� �����
struct BattaryRemainingLifeTimeInfo
{
	unsigned short Days;	// ���������� ����������� ���� ������ �������
	unsigned char Charge;	// ����� (������� ��������), %
	BattaryRemainingLifeTimeInfo()
	{
		Days = 0;
		Charge = 0;
	}
};

// ���������� � �������
struct BattaryInfoStruct
{
	BattaryType Type;				// ��� �������
	unsigned char Serial[8];		// �������� �����
	float Capacity;					// ��������� ������� ������� � ��
	unsigned char SectionsCount;	// ���������� ������
	unsigned char CurrentSection;	// ������������ ������ ������� � ����
	
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

// ��������� �������
struct BattaryStateStruct
{
	float ExpendedCapacity;		// ��������������� ����� (��)
	float Charge;				// ���������� ����� (%)
	float Temperature;			// ����������� (������� �������)
	float Voltage;				// ���������� (�)
	
	BattaryStateStruct()
	{
		ExpendedCapacity = 0.0;
		Charge = 0.0;
		Temperature = 0.0;
		Voltage = 0.0;
	}
};

// ���������� � ����������� �������� � ���������
struct BattaryConsumerInfoStruct
{
	unsigned char DeviceType;	// ��� ��
	unsigned short Serial;		// �������� ����� (SerialLow)
	char SoftwareVersion[15];	// ������ ��
	
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

// ������ ������ ������������ (������ �����)
struct BattaryConsumersListRecordPartOneStruct
{
	unsigned char RecordNumber;		// ����� ������
	unsigned char PartNumber;		// ����� ����� ������ (� 0)
	unsigned long ConnectTime;		// ����� �����������, ������� �� ������ ��������� (���.)
	unsigned long DisconnectTime;	// ����� ����������, ������� �� ������ ��������� (���.)
	float ExpendedCapacity;			// ��������������� ����� (��)
	float Discharge;				// ��������������� ����� (%)
	
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

// ������ ������ ������������ (������ �����)
struct BattaryConsumersListRecordPartTwoStruct
{
	unsigned char RecordNumber;		// ����� ������
	unsigned char PartNumber;		// ����� ����� ������ (� 1)
	BattaryConsumerInfoStruct Info;	// ���������� � �����������
	
	BattaryConsumersListRecordPartTwoStruct()
	{
		RecordNumber = 0;
		PartNumber = 1;
	}
};

// ���������� � ��������� ������������ ���������
struct BattaryResetCountersStruct
{
	unsigned short SoftResetCount;	// ������� ����������� ������������
	unsigned short HardResetCount;	// ������� ���������� ������������
	
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
	unsigned char RecordNumber;	// ����� ������
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
	bool Ack;	// ������������� ������ ������ � �����������
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
	BattaryInfoStruct Info;	// ���������� � �������
public:
	BattaryResponseVip1026Info()
	{
		_cmd = BattaryCommandVip1026Info;
	}
};

class BattaryResponseVip1026State : public BattaryResponseBase
{
public:
	unsigned long UpTime;		// �����  ��������� � ������� ������� (���.)
	BattaryStateStruct State;	// ��������� �������
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
	unsigned char RecordsCount;							// ������� ����� ������� � ������
	BattaryConsumersListRecordPartOneStruct PartOne;	// ������ ����� ������
	BattaryConsumersListRecordPartTwoStruct PartTwo;	// ������ ����� ������
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
	BattaryResetCountersStruct ResetCounters;	// ���������� � ��������� ������������
public:
	BattaryResponseVip1026ResetCounters()
	{
		_cmd = BattaryCommandVip1026ResetCounters;
	}
};

/// ������� ����� ��������� ����� �������
class BattaryBase
{
public:
	// ��� ������� ��������� ������ ��� ��������� ������ �� ���������
	typedef void (OnResponse)(bool result, BattaryResponseBase * response);
public:
	/// ����������� �� ���������
	BattaryBase()
	{

	}
	
	/// ����������
	virtual ~BattaryBase()
	{

	}
	
	/// ��� �������
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
	
	/// ��������� ������
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