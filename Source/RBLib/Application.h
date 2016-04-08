///////////////////////////////////////////////////////////////////////////////
//	�������� ����� ����������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef APPLICATION_H
#define APPLICATION_H

#include "Communication\CommunicationModule.h"
#include "Communication\InterfaceSimple.h"
#include "Protocol\Protocol.h"
#include "Protocol\CommandHandlerBase.h"
#include "EventDispetcher.h"
#include "GlobalVars.h"
#include "SysConfigurationRegisters.h"
#include "SpecialFunctionRegisters.h"
#include "Wdt.h"
#include "OperatingModeWise.h"
#include "ClockSystem.h"

namespace Rblib
{
  
///////////////////////////////////////////////////////////////
///	\brief ����� ���������� ����������� �������
///	
///	��� �������� ���������� ����� ������ ���������� ���������� ����������� �������
///////////////////////////////////////////////////////////////
class WdtKiller
{
public:
	/// ����������� ��-���������
	/*! ��� ������ ����� ������ ��������� ������, �.�. ������ �����������,
		����� �� ���������� ���������� ����������� �������.
	*/
	WdtKiller()
	{
#ifdef __ICC430__
		Rblib::Wdt::Stop();
#endif
	}
};

//
// �����, ���������� ��� �� ���������� ����� Sleep() �������
// � ����������� �� �������� ��������� _sleep
//
/////////////////////////////////////////////////////////////////////////////////////////////
///	\brief �����, ���������� ��� �� ���������� ����� Sleep() �������
///
///	\tparam SensorClass	����� ����������������� ����������.
///	\tparam _sleep		������� � ����� ���.
///
///	� ����������� �� ��������� ������� _sleep
///	������������ ������������� SleepInvokerImpl, ���������� ����� SensorClass::Sleep()
///	��� �� ���������� ���.
///
///	� ������ ������ ����� SensorClass ������ ������������� ����� Sleep(void).
///
///	\sa SleepInvokerImpl<SensorClass, false>
///	\sa SleepInvokerImpl<SensorClass, true>
///	\sa Application
/////////////////////////////////////////////////////////////////////////////////////////////
template<class SensorClass, bool _sleep>
class SleepInvokerImpl
{
};

////////////////////////////////////////////////
///	\brief ����� ����������.
///
///	\tparam Params		��������� ����������
///	\tparam SensorClass	����� ����������������� ����������
///	\tparam _sleep		��������� ������ ���. ���� true - �� ���������� ������� � ����� ��� � ����� ������ Run. ����� �� ����������� ������� � ����� ���.
///
///	�������� ����� ����������������� ����������.
///	��������� �������� ��������� � ������ ������ ����������������� ����������.
///
////////////////////////////////////////////////
template<class Params, class SensorClass, bool _sleep = false>
class Application
{
protected:
	/// ������� ������ �������� � ����� ���
	typedef SleepInvokerImpl<SensorClass, _sleep> SleepInvoker;
  
protected:
	/// ��������� ������� ������������ ���������� ������
	WdtKiller _wdtKiller;
	/// ��������� ������ ����������������� ����������
	SensorClass _sensor;

public:
	/// ������������� ����������
	/*!	���� ����� ������ ���� ������ � ������� main ����������������� ���������� �� ������� ��������� �����.
		� ���� ������ ����������� �������� �� ���� �� ������������ �� WatchDog.
		���� ����, �� �������������	������� ������������ �� WatchDog.
		����� ����������� �������� ����������������� ����������� ������.
		� ����� ����������� ����� ������ Init ����������������� ������ ����������.
	*/
    
    SensorClass& Sensor()
    {
        return _sensor;
    }
    
	void Init()
	{
#if RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5340 || \
	RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5342 || \
	RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5435A || \
	RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5438A || \
	RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5659
		// ������� ��� ����������� ������� ����������� ��� �������� Msp430f543x
		if (SysConfigurationRegisters::GetLastResetReason() == SysConfigurationRegisters::ResetReasonWdtTimeOut)
			Singletone<GlobalVars>::Instance().IncWatchdogResetCount();
		else
			Singletone<GlobalVars>::Instance().ClearWatchdogResetCount();
#elif RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F2618 || \
	  RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F1611
		// ������� ��� ����������� ������� ����������� ��� �������� Msp430f2xxx
		if (SpecialFunctionRegisters::GetWatchdogFlag())
		{
			Singletone<GlobalVars>::Instance().IncWatchdogResetCount();
		}
		else
			Singletone<GlobalVars>::Instance().ClearWatchdogResetCount();
		// �������� ���� ������ �� Wdt
		SpecialFunctionRegisters::ClearWatchdogFlag();
#else
	RBLIB_COMPILE_TIME_WARNING("Watchdog reset counter not supported for selected platform")
#endif
		Protocol::CommandHandlerBase::SetExternalCommandHandler(Protocol::ExternalCommandHandlerFunctor(_sensor));
		_sensor.Init();
	}

	/// ��� ��������� ����� ����������
	/*!	���� ����� ����� ��������� ���������� ��� ���������� ��������� ����� ������ ����������������� ����������.
		\remark ������ ��� �������� � ����������� ����� ������ main().
	*/
	void Run()
	{
#if RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5340 || \
	RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5342 || \
	RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5435A || \
	RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5438A || \
	RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F2618 || \
	RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F1611 || \
	RBLIB_PALTFORM_AUTODETECT_ID == RBLIB_PLATFORM_ID_MSP430F5659

		ClockSystem::CheckErrors();
#else
	RBLIB_COMPILE_TIME_WARNING("ClockSystem not supported for selected platform")
#endif
		Singletone<typename SensorClass::CommunicationModule>::Instance().ReadInterfaces();
		Singletone<typename SensorClass::CommunicationModule>::Instance().ProcessPackets();
		// ��������� ������� ���� �����������
		Singletone<EventDispetcher>::Instance().DispatchMessages();
		// ������ �������
		_sensor.Run();
		// �������� ��������� �������
		Singletone<typename SensorClass::CommunicationModule>::Instance().SendPackets();

		// ������, ���� ���������
		SleepInvoker::Sleep(_sensor);
	}

};

/// \brief ������������� SleepInvokerImpl, �� �������� ������� � ����� ���
///
///	\sa SleepInvokerImpl<SensorClass, true>
template<class SensorClass>
class SleepInvokerImpl<SensorClass, false>
{
public:
	/// ������� � ����� ���
	/*! � ������ ������������� ������� ���������� ������ ������.
	*/
	static inline void Sleep(SensorClass& sensor)
	{
	}
};

/// \brief ������������� SleepInvokerImpl, �������� ������� � ����� ���
///
///	\sa SleepInvokerImpl<SensorClass, false>
template<class SensorClass>
class SleepInvokerImpl<SensorClass, true>
{
public:
	/// ������� � ����� ���
	/*!
		\param sensor ������ ������ ����������������� ���������� (������ ���� ���������� ����� void Sleep(void))

		� ������ ������������� ������� ���������� ����� sensor.Sleep(), � ������� ������ ���� �������� ������� � ����� ���.
	*/
	static inline void Sleep(SensorClass& sensor)
	{
		sensor.Sleep();
	}
};

}

#endif
