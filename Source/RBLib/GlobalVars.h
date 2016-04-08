///////////////////////////////////////////////////////////////////////////////
//	����� ���������� ����������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GLOBALVARS_H
#define GLOBALVARS_H

#include "Utils\Singletone.h"

namespace Rblib
{

// ������� ��� ������������ �������� ������������
// �� �� ������ ������������������ ��� ������������
template<class CounterType>
class WatchdogResetCounter
{
protected:
	// ������� ������������ �� ����������� �������
	RBLIB_NO_INIT static CounterType _watchdogResetCount;
};

// ����� ���������� ����������
class GlobalVars :
	public WatchdogResetCounter<unsigned long>
{
protected:	
	// ���������� � ������
	struct RuntimeVars
	{
		unsigned long UpTime;	// ����� ����� ����������
		// ����������� �� ���������
		RuntimeVars()
		{
			UpTime = 0;
		}
	};
	
	// ���������� ���������� � ������
	RuntimeVars _runtimeVars;

	// ���� ���������
	friend class Singletone<EventDispetcher>;

public:
	// �������� ������� ������������ �� Watchdog
	void ClearWatchdogResetCount()
	{
		_watchdogResetCount = 0;
	}

	// ��������� ������� ������������ �� Watchdog
	void IncWatchdogResetCount()
	{
		_watchdogResetCount++;
	}

	// �������� ������� ������������ �� Watchdog
	unsigned long GetWatchdogResetCount()
	{
		return _watchdogResetCount;
	}

	// �������� ������� ������� �����
	void ClearUpTimeCounter()
	{
		_runtimeVars.UpTime = 0;
	}

	// ��������� ������� ������� �����
	void IncUpTimeCounter()
	{
		_runtimeVars.UpTime++;
	}

	// �������� ������� ������� �����
	unsigned long GetUpTimeCounter()
	{
		return _runtimeVars.UpTime;
	}
};

// ��������������� ����������� ����������
template<class CounterType>
CounterType WatchdogResetCounter<CounterType>::_watchdogResetCount;

}

#endif