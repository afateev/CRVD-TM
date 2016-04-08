////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		27/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef DRIVECONTROLLERSWITCH_H
#define DRIVECONTROLLERSWITCH_H

template <class Port, unsigned char Pin1, unsigned char Pin2>
class DriveControllerSwitch
{
public:
	static void Init()
	{
		// ���� �����������
		Port::SetInputWithPullUpDownPin(Pin1);
		Port::SetInputWithPullUpDownPin(Pin2);
		// � �������
		Port::SetBit(Pin1);
		Port::SetBit(Pin2);
	}
	
	/*	���� ������� �������� ����� ��������� ������
		����� ��� ������� false, ������ ��������� ����� ������, �������� Atmega �� ��������
	*/
	
	// �������� ������
	static bool IsPrimaryActive()
	{
		bool pin1 = Port::Read(Pin1);
		bool pin2 = Port::Read(Pin2);
		
		return pin1 && !pin2;
	}
	
	// ��������� ������
	static bool IsReserveActive()
	{
		bool pin1 = Port::Read(Pin1);
		bool pin2 = Port::Read(Pin2);
		
		return !pin1 && pin2;
	}
};

#endif 