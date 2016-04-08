///////////////////////////////////////////////////////////////////////////////
//	����� �������� ����������� ������� �������
//	������������ �������� ���������� ����� ������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef BATTARYREPORTER_H
#define BATTARYREPORTER_H

namespace Rblib
{
namespace StdDevice
{

///////////////////////////////////////////////////////////////
///	\brief ��������� ��� �������� �������� ������
///	
///////////////////////////////////////////////////////////////
struct BattaryReporterStorageDataType
{
	struct DataStruct
	{
		unsigned long ReportPeriod;			// ������ ������� ����������� �� ���������� ������� ������ ������� (������)
	};
	/// ������
	DataStruct Data;
	/// ����������� �����
	unsigned char Crc;

	/// ������ ����������� ����� �� ���� ������
	unsigned char CalcCrc()
	{
		unsigned char *data = (unsigned char *)&Data;
		unsigned char crc = 0x89;

		for (unsigned char i = 0; i < sizeof(Data); i++)
			crc += data[i];

		return crc;
	}
};

///////////////////////////////////////////////////////////////
///	\brief ����� �������� ����������� �� ���������� ������� ������ �������
///	
///	\tparam Battary							����� �������� �������
///	\tparam Container						��� ���������� (������ ���� Rblib::Flash ���� �����������)
///	\tparam tickEvent						����������� �������
///	\tparam BatickPeriodMilisecondsttary	������ ������������ ������� (��)
///	\tparam interfacePcId					������������� ���������� ���
///	\tparam interfaceModemId				������������� ���������� ������
///
///////////////////////////////////////////////////////////////
template<class Battary, class Container, unsigned int tickEvent, unsigned int tickPeriodMiliseconds, unsigned int interfacePcId, unsigned int interfaceModemId>
class BattaryReporter
{
protected:
	static bool _inited;

	static const unsigned long OneMinuteMaxCount = 60000 / tickPeriodMiliseconds;	// ������� ����������� ������� ��������� � ������ (����� � ������������ / ������ ����������� �����)
	static const unsigned long DefaultReportPeriod = 1140;							// ������ �������� ����������� �� ��������� (1140 ����� = �����)

	static unsigned long _oneMinuteCounter;
	static unsigned long _reportCounter;
public:
	/// �������������
	static void Init()
	{
		if (_inited)
			return;
		
		_oneMinuteCounter = 0;
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
		// ������ ��������
		case Rblib::Protocol::CmdBattaryGetRemainingLifeTimeReportPeriod:
			{
				unsigned long period = GetReportPeriod();
				
				// ����� ��������� �� ������ �������
				Rblib::Protocol::CmdInfo answer(*cmd);
				// ������� ������
				answer.Cmd = Rblib::Protocol::CmdBattaryNotifyRemainingLifeTimeReportPeriod;
				// � ���� ������ ������ ������
				answer.Data = (unsigned char *)&period;
				answer.DataSize = sizeof(period);
				// ��������� �����
				outputInterface->SendCommand(&answer);
				// ������� ����������
				cmd->Handled = true;
			}
			break;
		// ��������� ��������
		case Rblib::Protocol::CmdBattarySetRemainingLifeTimeReportPeriod:
			{
				unsigned long period = 0;
				
				// ����� ��������� �� ������ ��������� �������
				Rblib::Protocol::CmdInfo answer(*cmd);
				// ��������� ������ ������
				unsigned char requiredDataSize = sizeof(period);
				// ��-��������� ������� �������
				answer.Cmd = Rblib::Protocol::CmdBattarySetRemainingLifeTimeReportPeriodNak;
				// ���� ���� ������ � ������ ����������
				if (0 != cmd->Data && cmd->DataSize >= requiredDataSize)
				{
					// �������� ������
					memcpy(&period, cmd->Data, requiredDataSize);
					// ���������
					SetReportPeriod(period);
					// ����� "�������"
					answer.Cmd = Rblib::Protocol::CmdBattarySetRemainingLifeTimeReportPeriodAck;
					// ���������� ����� "�������������"
					if (outputInterface)
						outputInterface->SendCommand(&answer);
					// �������� ������� ��� ������������
					cmd->Handled = true;
					return;
				}
				else
				{
					// ���������� ����� "�����"
					if (outputInterface)
						outputInterface->SendCommand(&answer);
					// �������� ������� ��� ������������
					cmd->Handled = true;
				}
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
			_oneMinuteCounter++;
			if (_oneMinuteCounter >= OneMinuteMaxCount)
			{
				_oneMinuteCounter = 0;
				_reportCounter++;
				unsigned long reportPeriod = GetReportPeriod();
				if (_reportCounter >= reportPeriod)
				{
					_reportCounter = 0;
					if (reportPeriod > 0)
						Report();
				}
			}
		}
	}
protected:
	// ��������� �����������
	static void Report()
	{
		// �� ������
		Battary::SendRemainingLifeTimeReport(interfacePcId, -1);
		// �� ����� �� ����� 0
		Battary::SendRemainingLifeTimeReport(interfaceModemId, 0);
	}

	// �������� ����������� �������� "������ ������� �����������"
	static unsigned long GetReportPeriod()
	{
		// ������ �� ���������� (������)
		BattaryReporterStorageDataType data = Container::Read();
		// ��������� �����������
		// ���� �� ������ ����� ������, �� �� ���������
		if (data.CalcCrc() != data.Crc)
			return DefaultReportPeriod;
		
		return data.Data.ReportPeriod;
	}

	// ��������� �������� "������ ������� �����������"
	static void SetReportPeriod(unsigned long period)
	{
		// �������������� ������
		BattaryReporterStorageDataType data;

		data.Data.ReportPeriod = period;
		data.Crc = data.CalcCrc();

		// ���������� � ���������
		Container::Write(data);
	}
};

// ��������������� ����������� ����������
template<class Battary, class Container, unsigned int tickEvent, unsigned int tickPeriodMiliseconds, unsigned int interfacePcId, unsigned int interfaceModemId>
bool BattaryReporter<Battary, Container, tickEvent, tickPeriodMiliseconds, interfacePcId, interfaceModemId>::_inited = false;

template<class Battary, class Container, unsigned int tickEvent, unsigned int tickPeriodMiliseconds, unsigned int interfacePcId, unsigned int interfaceModemId>
unsigned long BattaryReporter<Battary, Container, tickEvent, tickPeriodMiliseconds, interfacePcId, interfaceModemId>::_oneMinuteCounter = 0;

template<class Battary, class Container, unsigned int tickEvent, unsigned int tickPeriodMiliseconds, unsigned int interfacePcId, unsigned int interfaceModemId>
unsigned long BattaryReporter<Battary, Container, tickEvent, tickPeriodMiliseconds, interfacePcId, interfaceModemId>::_reportCounter = 0;

}
}

#endif