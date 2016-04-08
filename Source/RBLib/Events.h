/// \file
///	\brief ���� ������� � ����� �������.
///	


#ifndef EVENTS_H
#define EVENTS_H

/// ������������ ��� ����������� RBlib.
namespace Rblib
{
	/// ��� ����� �������.
	typedef unsigned char EventCodeType;
	/// ��� ��������� 1.
	typedef unsigned char EventParam1Type;
	/// ��� ��������� 2.
	typedef unsigned char EventParam2Type;
	/// ��� ��������� 3.
	typedef unsigned char EventParam3Type;
	/// ��� ��������� 4.
	typedef unsigned char EventParam4Type;

	/// \brief ���� �������.
	typedef enum
	{
		/// ������ �������, ������ �� �������� (��� ������).
		EventEmty = 0,
#ifndef __ICCARM__
		/// ���������� ����� �� ��������� "����".\n
		/// Param1 - ����� �����.\n
		/// Param2 - ����� "����".\n
		/// Param3 - ��������� "����".
		EventGpioPinChanged = 1,
		/// ������������ �������.\n
		/// Param1 - ����� �������.
		EventTimerOverflow = 2,
		/// ������� ������� ������ �������� ���������.\n
		/// Param1 - ����� �������.\n
		/// Param2 - ����� �������� ���������.
		EventTimerCompare = 3,		
		/// ���������� �� Uart.\n
		/// Param1 - ����� ����������.
		EventUart = 4,
		/// ���������� ������ ���������� ������������.
		EventClockErrorsSet = 5,
		/// ��� ����� ������ ���������� ������������ ����������.
		EventClockErrorsClear = 6,
#endif
		/// ������ ���� I2C
		/// Param1 - ����� ����������.
		EventI2CError = 7,
        /// I2C, ��������� ����������� �������� ������.
        /// Param1 - ����� ����������.
        /// Param2 - ��� ������.
        EventI2CAsynchWriteComplete = 8,
        /// I2C, ��������� ����������� �������� ������.
        /// Param1 - ����� ����������.
        /// Param2 - ��� ������.
        EventI2CAsynchReadComplete = 9,
		/// ����������� Uart � ������ ������ ������
		/// Param1 - ����� ����������.
		EventUartReceiveStartNotify = 10,
		/// ����������� Uart � ������ �������� ������
		/// Param1 - ����� ����������.
		EventUartTransmitStartNotify = 11,
		/// ������� ������������ ���������� ������������
		/// Param1 - ��� GUI �������
		/// Param2..3 - ��������� GUI �������
		EventGui = 12,
        /// ������� ������� �������
        /// Param1 - �������� ���������� �������������
        EventFilterEvent = 13,
		/// ������ ���������������� �������.
		EventUser = 14
	} EventCode;

	/// �������.
	class Event
	{
	public:	
		/// ��� �������.
		EventCode Code;
		/// �������� ������� ��������� �� ����.
		EventParam1Type Param1;
		/// �������� ������� ��������� �� ����.
		EventParam2Type Param2;
		/// �������� ������� ��������� �� ����.
		EventParam3Type Param3;
		/// �������� ������� ��������� �� ����.
		EventParam4Type Param4;
	private:
	public:

		/// ����������� �� ���������
		Event()
		{}
        
		/// ����������� �� ���� �������.
		Event(EventCode code):
			Code(code), Param1(0), Param2(0), Param3(0), Param4(0)
		{}
		/// ����������� �� ���� + 1 ��������.
		Event(EventCode code, EventParam1Type param1):
			Code(code), Param1(param1), Param2(0), Param3(0), Param4(0)
		{}
		/// ����������� �� ���� + 2 ���������.
		Event(EventCode code, EventParam1Type param1, EventParam2Type param2):
			Code(code), Param1(param1), Param2(param2), Param3(0), Param4(0)
		{}
		/// ����������� �� ���� + 3 ���������.
		Event(EventCode code, EventParam1Type param1, EventParam2Type param2, EventParam3Type param3):
			Code(code), Param1(param1), Param2(param2), Param3(param3), Param4(0)
		{}
		/// ����������� �� ���� + 4 ���������.
		Event(EventCode code, EventParam1Type param1, EventParam2Type param2, EventParam3Type param3, EventParam4Type param4):
			Code(code), Param1(param1), Param2(param2), Param3(param3), Param4(param4)
		{}
		
		/// ����������� ��� ���� ������� � ���� ������ �����.
		/// \details ������ ��� ���������������� �������.
		Event(unsigned int code):
			Code((EventCode)code), Param1(0), Param2(0), Param3(0), Param4(0)
		{}
		/// ����������� �� ���� + 1 ��������
		Event(unsigned int code, EventParam1Type param1):
			Code((EventCode)code), Param1(param1), Param2(0), Param3(0), Param4(0)
		{}
		/// ����������� �� ���� + 2 ���������.
		Event(unsigned int code, EventParam1Type param1, EventParam2Type param2):
			Code((EventCode)code), Param1(param1), Param2(param2), Param3(0), Param4(0)
		{}
		/// ����������� �� ���� + 3 ���������.
		Event(unsigned int code, EventParam1Type param1, EventParam2Type param2, EventParam3Type param3):
			Code((EventCode)code), Param1(param1), Param2(param2), Param3(param3), Param4(0)
		{}
		/// ����������� �� ���� + 4 ���������.
		Event(unsigned int code, EventParam1Type param1, EventParam2Type param2, EventParam3Type param3, EventParam4Type param4):
			Code((EventCode)code), Param1(param1), Param2(param2), Param3(param3), Param4(param4)
		{}

		/// ����������� �� ���� �������.
		void Set(EventCode code)
		{
			Code = code;
			Param1 = 0;
			Param2 = 0;
			Param3 = 0;
			Param4 = 0;
		}

		/// ����������� �� ���� + 1 ��������.
		void Set(EventCode code, EventParam1Type param1)
		{
			Code = code;
			Param1 = param1;
			Param2 = 0;
			Param3 = 0;
			Param4 = 0;
		}

		/// ����������� �� ���� + 2 ���������.
		void Set(EventCode code, EventParam1Type param1, EventParam2Type param2)
		{
			Code = code;
			Param1 = param1;
			Param2 = param2;
			Param3 = 0;
			Param4 = 0;
		}

		/// ����������� �� ���� + 3 ���������.
		void Set(EventCode code, EventParam1Type param1, EventParam2Type param2, EventParam3Type param3)
		{
			Code = code;
			Param1 = param1;
			Param2 = param2;
			Param3 = param3;
			Param4 = 0;
		}
		
		/// ����������� �� ���� + 4 ���������.
		void Set(EventCode code, EventParam1Type param1, EventParam2Type param2, EventParam3Type param3, EventParam4Type param4)
		{
			Code = code;
			Param1 = param1;
			Param2 = param2;
			Param3 = param3;
			Param4 = param4;
		}

        bool operator == (const Event& e) const
        {
            return (Code == e.Code && Param1 == e.Param1 && Param2 == e.Param2 && Param3 == e.Param3 && Param4 == e.Param4);
        }

        bool operator != (const Event& e) const
        {
            return (Code != e.Code || Param1 != e.Param1 || Param2 != e.Param2 || Param3 != e.Param3 || Param4 != e.Param4);
        }

	};

	// ������ ����������� �������.
	template<class T>
	class EventHandlerClass
	{
		T *handler;
	public:
  		EventHandlerClass(T *object) : handler(object)
		{
		}
  		/// �������� ��������� �������.
		void operator()(const Event &e)
		{
			handler->EventHandler(e);
		}
	};
}

#endif // EVENTS
