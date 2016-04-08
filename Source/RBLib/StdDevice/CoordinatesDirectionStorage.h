///////////////////////////////////////////////////////////////////////////////
//	����� �������� � ������ �������������� ��������� ����������� ����������
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef COORDINATESDIRECTIONSTRORAGE_H
#define COORDINATESDIRECTIONSTRORAGE_H

namespace Rblib
{
namespace StdDevice
{

///////////////////////////////////////////////////////////////
///	\brief ��������� ��� �������� ��������� ����������� ���������� � ����������������� ������
///	
///////////////////////////////////////////////////////////////
struct CoordinatesDirectionStorageDataType
{
	struct DataStruct
	{
		float Lat;			// ������
		float Lon;			// �������
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
///	\brief ����� �������� � ������ �������������� ��������� ����������� ����������
///	
///	\tparam Container	��� ���������� (������ ���� Rblib::Flash ���� �����������)
///
///////////////////////////////////////////////////////////////
template<class Container>
class CoordinatesDirectionStorage
{
protected:
	// ������ ������ ������������ ������ ������
	#pragma pack(push, 1)
	struct CmdDataFormat
	{
		unsigned char DeviceNumber;		// ����� ����������
		float Lat;						// ������
		float Lon;						// �������
	};
	#pragma pack(pop)
	
	unsigned char _myNumber;	// ����� ���������� � ����, ������� ������������ ������ �������

	static const unsigned char _myNumberInvalid = 0xFF;

	CmdDataFormat _cmdData;
public:
	
	// ����������� �� ���������
	CoordinatesDirectionStorage() : _myNumber(_myNumberInvalid)
	{
	
	}
	
	// �������� "���� �����"
	///	���������� ����� "������" ����������
	/*!
		\param myNumber ����� ���������� � ���� (�����).
	*/
	void CoordinatesDirectionStorageSetMyNumber(unsigned char myNumber)
	{
		_myNumber = myNumber;
	}
	
	///	���������� ������ ������� � ���������� ���������
	/*!
		\param cmd �������.
		\param outputInterface ��������� ��� �������� ������.
	*/
	void CoordinatesDirectionStorageCommandHandler(Rblib::Protocol::CmdInfo *cmd, Rblib::Communication::InterfaceBase *outputInterface)
	{
		switch(cmd->Cmd)
		{
		// ������ ��������� �����������
		case Rblib::Protocol::CmdGetCoordinatesDirection:
			{
  				// ���� �� ����� ���� �����, �� ������ �� ����� �������
				if (_myNumberInvalid == _myNumber)
					return;

				// ����� ��������� �� ������ �������
				Rblib::Protocol::CmdInfo answer(*cmd);
				
				// ����� ���������� ��� �������� ������������� ����������
				unsigned char deviceNumber;
				// ��������� ������ ������
				unsigned char requiredDataSize = sizeof(deviceNumber);
				// ���� ���� ������ � ������ ����������
				if (0 != cmd->Data && cmd->DataSize >= requiredDataSize)
				{
					// �������� ������
					memcpy(&deviceNumber, cmd->Data, requiredDataSize);
					// ����� ������ ���� _myNumber, �.�. ���������� ���
					if (_myNumber == deviceNumber)
					{
						float Lat = 0;
						float Lon = 0;
						// ���� ���������� ����
						if (GetFromContainer(Lat, Lon))
						{
							// �������������� ������ ��� ��������
							_cmdData.DeviceNumber = _myNumber;
							_cmdData.Lat = Lat;
							_cmdData.Lon = Lon;
						  // ������� ������
							answer.Cmd = Rblib::Protocol::CmdNotifyCoordinatesDirection;
							// ���� ������
							answer.Data = (unsigned char *)&_cmdData;
							answer.DataSize = sizeof(CmdDataFormat);
							// ��������� �����
							outputInterface->SendCommand(&answer);
						}
						// ������� ����������
						cmd->Handled = true;
					}
				}
			}
			break;
		// ��������� ��������� �����������
		case Rblib::Protocol::CmdSetCoordinatesDirection:
			{
				// ���� �� ����� ���� �����, �� ������ �� ����� �������
				if (_myNumberInvalid == _myNumber)
					return;
				
				// ����� ��������� �� ������ ��������� �������
				Rblib::Protocol::CmdInfo answer(*cmd);
				// ��������� ������ ������
				unsigned char requiredDataSize = sizeof(CmdDataFormat);
				// ��-��������� ������� �������
				answer.Cmd = Rblib::Protocol::CmdSetCoordinatesDirectionNak;
				// ���� ���� ������ � ������ ����������
				if (0 != cmd->Data && cmd->DataSize >= requiredDataSize)
				{
					// �������� ������
					memcpy(&_cmdData, cmd->Data, requiredDataSize);
					
					// ��������� �������� ������
					// ����� ������ ���� _myNumber, �.�. ���������� ����, � �� ���-�� ���
					if (_myNumber == _cmdData.DeviceNumber)
					{
						// ��������� ����� ��������
						SetToContainer(_cmdData.Lat, _cmdData.Lon);
						// ����� "�������"
			  			answer.Cmd = Rblib::Protocol::CmdSetCoordinatesDirectionAck;
						// ���������� ����� "�������������"
						if (outputInterface)
							outputInterface->SendCommand(&answer);
						// �������� ������� ��� ������������
						cmd->Handled = true;
						return;
					}
					// ���� ����� �� ���, �� ����� �� ����� ��������� � ������� ����� ���� ���������� ������ �������
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
protected:
	// �������� ������ �� ����������
	bool GetFromContainer(float &Lat, float &Lon)
	{
		// ������
		CoordinatesDirectionStorageDataType data = Container::Read();
		// ��������� �����������
		if (data.CalcCrc() != data.Crc)
			return false;
		
		// ����������
		Lat = data.Data.Lat;
		Lon = data.Data.Lon;
		return true;
	}

	// ��������� ������ � ���������
	void SetToContainer(float Lat, float Lon)
	{
		// �������������� ������
		CoordinatesDirectionStorageDataType data;

		data.Data.Lat = Lat;
		data.Data.Lon = Lon;
		data.Crc = data.CalcCrc();

		// ���������� � ���������
		Container::Write(data);
	}
};

/*!
\class CoordinatesStorage

����� ��������� �������� � ���������� ����������������� ���������� �����������
���������������� �� ���������� � ������ �������������� ���������.

��� �������� ��������� ����� ���������� ���������, ����������� ����� �������� ������� <b>Container</b>.

����������� ������ ���� ����� Rblib::Flash ��� �����������, �������� ������ � ���� ��������� CoordinatesStorageDataType, ��������:
\code
typedef Rblib::Flash<Rblib::StdDevice::CoordinatesStorageDataType, 0> MyCoordContainer;
\endcode

��� ���������� ������� ������ � ������������ ���������� ������������ ����� ����������������� ���������� �� ������ CoordinatesStorage:
\code
// ����� ����������������� ����������
class DeviceClass: public Rblib::StdDevice::CoordinatesStorage<MyCoordContainer>
{
	...
};
\endcode

����� � ����������� ������ ������ ����������������� ���������� ���������� �������� ����� CoordinatesStorageCommandHandler():
\code
// ���������� ������
void DeviceClass::CommandHandler(CmdInfo *cmd, InterfaceBase *outputInterface)
{
	...
	CoordinatesStorageCommandHandler(cmd, outputInterface);
	...
}
\endcode

������� ������� � ���������� ��������� ��������� ����� ����������, ��� �������� ���������� ��� ����������. CoordinatesStorageCommandHandler() ��������� ���� ����� � ��������� �������
������ ��� ��� ������, � ������� ���� ������ ���������� ��������� � ������� �������� ������� "������" ����������. ���� ����� �������� ������� ������ CoordinatesStorageSetMyNumber().
������������ ������ �������� ��� ��� ��������� ������ ���������� �� ������:
\code
// ���������� ������
void DeviceClass::CommandHandler(CmdInfo *cmd, InterfaceBase *outputInterface)
{
	...
	case CmdModemNotifySelfInfo:
	{
		...
		netAddress = xxx;
		...
		CoordinatesStorageSetMyNumber(netAddress);
		...
	}
	break;
	...
}
\endcode



*/

/*!
\class CoordinatesStorageDataType

������� �� ���� ������, ����������� ����������, � ���� ����������� �����.

��� �������� ����������� ������, ���������� �������� <b>Crc</b> �� ���������, ������� ����� CalcCrc().

���� ���������� �� ���� �� ���� ��������, �� ��������, ��� ����������� ����� �� ������� ��������.

\sa CoordinatesStorage

*/

}
}

#endif