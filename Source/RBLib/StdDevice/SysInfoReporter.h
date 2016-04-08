///////////////////////////////////////////////////////////////////////////////
//	����� ������������ ����������� �������
//	������� "�������� ��������� ���������� (����� ������ � ���������� ������������)"
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef SYSINFOREPORTER_H
#define SYSINFOREPORTER_H

#include "..\Protocol\Protocol.h"
#include "..\Protocol\InternalCommands.h"

namespace Rblib
{
namespace StdDevice
{

///////////////////////////////////////////////////////////////
///	\brief ����� ��������� �������� ��������� ����������
///	
///	\tparam tickEvent						����������� �������
///	\tparam tickPeriodMiliseconds			������������� ����������� ������� (��)
///
///////////////////////////////////////////////////////////////
template<unsigned int tickEvent, unsigned int tickPeriodMiliseconds>
class SysInfoReporter
{
protected:
	#pragma pack(push, 1)
	struct ReportStruct 
	{
		unsigned long WorkTime;	// ����� ������
		unsigned int RebootCnt;	// ������� ������������
	};
	#pragma pack (pop)

	static bool _inited;

	static const unsigned long OneSecondMaxCount = 1000 / tickPeriodMiliseconds;	// ������� ����������� ������� ��������� � ������� (����� � ������������ / ������ ����������� �����)
	
	static unsigned long _oneSecondCounter;

	static unsigned long _upTime; // ����� ������ �� (���.)
public:
	/// �������������
	static void Init()
	{
		if (_inited)
			return;

		_oneSecondCounter = 0;
		_upTime = 0;
		Singletone<EventDispetcher>::Instance().AddHandler(tickEvent, EventDispetcher::EventFunctor(EventHandler), false);
		_inited = true;
	}
	
	///	���������� ������ ������� � ���������� ���������
	/*!
		\param cmd �������.
		\param outputInterface ��������� ��� �������� ������.
	*/
	static void CommandHandler(Rblib::Protocol::CmdInfo *cmd, Rblib::Communication::InterfaceBase *outputInterface)
	{
		if (!_inited)
			return;

		switch(cmd->Cmd)
		{
		// ������
		case Rblib::Protocol::CmdGetSystemInfo:
			{
				ReportStruct data;
				data.WorkTime = _upTime;
				data.RebootCnt = Singletone<GlobalVars>::Instance().GetWatchdogResetCount();
				
				// ����� ��������� �� ������ �������
				Rblib::Protocol::CmdInfo answer(*cmd);
				// ������� ������
				answer.Cmd = Rblib::Protocol::CmdNotifySystemInfo;
				// � ���� ������ ������ ������
				answer.Data = (unsigned char *)&data;
				answer.DataSize = sizeof(data);
				// ��������� �����
				outputInterface->SendCommand(&answer);
				// ������� ����������
				cmd->Handled = true;
			}
			break;
		}
	}

	///	���������� �������
	/*!
		\param event �������.
	*/
	static void EventHandler(const Event &event)
	{
		if (!_inited)
			return;

		if (tickEvent == event.Code)
		{
			_oneSecondCounter++;
			if (_oneSecondCounter >= OneSecondMaxCount)
			{
				_oneSecondCounter = 0;
				_upTime++;
			}
		}
	}
};

// ��������������� ����������� ����������
template<unsigned int tickEvent, unsigned int tickPeriodMiliseconds>
bool SysInfoReporter<tickEvent, tickPeriodMiliseconds>::_inited = false;

template<unsigned int tickEvent, unsigned int tickPeriodMiliseconds>
unsigned long SysInfoReporter<tickEvent, tickPeriodMiliseconds>::_oneSecondCounter = 0;

template<unsigned int tickEvent, unsigned int tickPeriodMiliseconds>
unsigned long SysInfoReporter<tickEvent, tickPeriodMiliseconds>::_upTime = 0;

}
}

#endif