////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		27/12/2011
////////////////////////////////////////////////////////////////////////////////

#ifndef DRIVEEVENTSGENERATOR_H
#define DRIVEEVENTSGENERATOR_H

#include "Events.h"
/*
template<unsigned char Reg, unsigned char Bit>
class BitTester
{
protected:
	enum State
	{
		StateUndefined,
		StateOne,
		StateZero
	};
	
	static State _state;
public:
};

template<class ActiveDriveControllerParams, class Events>
class DriveEventsGenerator
{
protected:
	enum DriveState
	{
		DSUndefined,
		DSRunning,
		DSStopped
	};
	
	static DriveState _state;
public:
	static void Run()
	{
		time_t curTime = Rtc::GetTime();
		// нет данных, нечего делать
		if (!ActiveDriveControllerParams::HasParams())
			return;
		
		switch(_state)
		{
		case DSUndefined:
			{
				if (ActiveDriveControllerParams::GetFlagEngineOn())
				{
					Event e(0, EventDriveStart);
					e.SetNoLog();
					Events::Push(e);
					_state = DSRunning;
				}
				else
				{
					Event e(0, EventDriveStop, ActiveDriveControllerParams::GetStopFlag());
					e.SetNoLog();
					Events::Push(e);
					_state = DSStopped;
				}
			}
			break;
		case DSRunning:
			{
				if (!ActiveDriveControllerParams::GetFlagEngineOn())
				{
					Events::Push(curTime, EventDriveStop, ActiveDriveControllerParams::GetStopFlag());
					_state = DSStopped;
				}
			}
			break;
		case DSStopped:
			{
				if (ActiveDriveControllerParams::GetFlagEngineOn())
				{
					Events::Push(curTime, EventDriveStart);
					_state = DSRunning;
				}
			}
			break;
		}
	}
};

template<class ActiveDriveControllerParams, class Events>
DriveEventsGenerator<ActiveDriveControllerParams, Events>::DriveState
DriveEventsGenerator<ActiveDriveControllerParams, Events>::_state = 
DriveEventsGenerator<ActiveDriveControllerParams, Events>::DSUndefined;
*/

template<class ActiveDriveControllerParams, class Events>
class DriveEventsGenerator
{
protected:
	static bool _stateUndefined;
	static unsigned short _prevDriveState;
	static unsigned short _prevProtectionState;
	
	enum ActiveController
	{
		ACUndefined,
		ACPrimary,
		ACReserve,
		ACEmergency
	};
	
	static ActiveController _prevActiveController;
public:
	static void Run()
	{
		time_t curTime = Rtc::GetTime();
		
		// нет данных, нечего делать
		if (_stateUndefined && !ActiveDriveControllerParams::HasParams())
			return;
		
		unsigned short curDriveState = ActiveDriveControllerParams::GetStateReg();
		unsigned short curProtectionState = ActiveDriveControllerParams::GetProtectionStateReg();
		ActiveController curActiveController = ActiveDriveControllerParams::IsPrimaryActive() ? ACPrimary : ACReserve;
		
		if (_prevActiveController != curActiveController)
		{
			switch (curActiveController)
			{
			case ACPrimary:
				{
					Event e(curTime, EventRunMain);
					Events::Push(e);
				}
				break;
			case ACReserve:
				{
					Event e(curTime, EventRunReserve);
					Events::Push(e);
				}
				break;
			}
		}
		
		//разбираем регистр защит
		for (unsigned char i = 0; i < 16; i++)
		{
			bool curState = (curProtectionState >> i) & 0x0001 == 0x0001;
			bool prevState = (_prevProtectionState >> i) & 0x0001 == 0x0001;
			bool stateChanged = curState != prevState;
			if (stateChanged || _stateUndefined)
			{
				Event e;
				switch(i)
				{
				// Асинхронный ход
				case ActiveDriveControllerParams::ProtAsynch:
					{
						if (curState)
							e = Event(0, EventProtAsynch);
					}
					break;
				case ActiveDriveControllerParams::ProtLostEnergizin:
					{
						if (curState)
							e = Event(0, EventProtLostEnergizin);
					}
					break;
				case ActiveDriveControllerParams::ProtWrongMode:
					{
						if (curState)
							e = Event(0, EventProtWrongMode);
					}
					break;
				case ActiveDriveControllerParams::ProtLongStart:
					{
						if (curState)
							e = Event(0, EventProtLongStart);
					}
					break;
				case ActiveDriveControllerParams::ProtKZ:
					{
						if (curState)
							e = Event(0, EventProtKZ);
					}
					break;
				case ActiveDriveControllerParams::ProtLowUst:
					{
						if (curState)
							e = Event(0, EventProtLowUst);
					}
					break;
				case ActiveDriveControllerParams::ProtLowIst:
					{
						if (curState)
							e = Event(0, EventProtLowIst);
					}
					break;
				case ActiveDriveControllerParams::ProtStator:
					{
						if (curState)
							e = Event(0, EventProtStator);
					}
					break;
				case ActiveDriveControllerParams::StopButton:
					{
						// кнопку игнорируем, только если двигатель не работал или было опробование
						if (((_prevDriveState >> ActiveDriveControllerParams::SfEngineOn) & 0x01 == 0) || ((_prevDriveState >> ActiveDriveControllerParams::SfCheckout) & 0x01 == 1))
							break;
						if (curState)
							e = Event(0, EventStopButton);
					}
					break;
				}
				if (!_stateUndefined)
				{
					e.SetDt(curTime);
					Events::Push(e);
				}
			}
		}
		
		// разбираем регистр состояний
		for (unsigned char i = 0; i < 16; i++)
		{
			bool curState = (curDriveState >> i) & 0x0001 == 0x0001;
			bool prevState = (_prevDriveState >> i) & 0x0001 == 0x0001;
			bool stateChanged = curState != prevState;
			if (stateChanged || _stateUndefined)
			{
				Event e;
				switch(i)
				{
				// возбуждение
				case ActiveDriveControllerParams::SfEnergizing:
					{
						if (_stateUndefined)
							break;
						if (curState)
							e = Event(0, EventEnergizingOn);	// вкл
						else
							e = Event(0, EventEnergizingOff);	// выкл
					}
					break;
				// форсировка
				case ActiveDriveControllerParams::SfForcing:
					{
						if (_stateUndefined)
							break;
						if (curState)
							e = Event(0, EventForcingOn);	// вкл
						else
							e = Event(0, EventForcingOff);	// выкл
					}
					break;
				// ограничение минимального возбудения
				case ActiveDriveControllerParams::SfOmv:
					{
						if (_stateUndefined)
							break;
						if (curState)
							e = Event(0, EventOmvOn);	// вкл
						else
							e = Event(0, EventOmvOff);	// выкл
					}
					break;
				// управление по cos или току ротора
				case ActiveDriveControllerParams::SfCosControl:
					{
						// только если не ручное
						if (_stateUndefined || ActiveDriveControllerParams::GetFlagRControl())
							break;
						if (curState)
							e = Event(0, EventCosControl);	// по косинусу
						else
							e = Event(0, EventRotorCurrentControl);	// по току ротора
					}
					break;
				// ручное управление
				case ActiveDriveControllerParams::SfRControl:
					{
						if (_stateUndefined)
							break;
						if (curState)
							e = Event(0, EventRControl);	// ручное включено
						else
						{
							// если не ручное, то какое
							if (ActiveDriveControllerParams::GetFlagCosControl())
								e = Event(0, EventCosControl);	// по косинусу
							else
								e = Event(0, EventRotorCurrentControl);	// по току ротора
						}
					}
					break;
				// нагрев ротора
				case ActiveDriveControllerParams::SfHeatRot:
					{
						if (_stateUndefined)
							break;
						if (curState)
							e = Event(0, EventHeatRot);	// вкл
					}
					break;
				// охлаждение ротора
				case ActiveDriveControllerParams::SfCoolRot:
					{
						if (_stateUndefined)
							break;
						if (curState)
							e = Event(0, EventCoolRot);	// вкл
					}
					break;
				// асинхронный ход
				case ActiveDriveControllerParams::SfAsynch:
					{
						if (_stateUndefined)
							break;
						if (curState)
							e = Event(0, EventAsynch);	// вкл
					}
					break;
				// двигатель включен
				case ActiveDriveControllerParams::SfEngineOn:
					{
						// если сейчас не опробование и было не опробование
						if (ActiveDriveControllerParams::GetFlagCheckout() && ((_prevDriveState >> ActiveDriveControllerParams::SfCheckout) & 0x01 == 0))
							break;
						if (curState)
						{
							e = Event(0, EventDriveStart);
						}
						else
						{
							e = Event(0, EventDriveStop, ActiveDriveControllerParams::GetStopFlag());
						}
					}
					break;
				// опробование
				case ActiveDriveControllerParams::SfCheckout:
					{
						if (_stateUndefined)
							break;
						if (curState)
							e = Event(0, EventCheckoutStart);	// вкл
						else
							e = Event(0, EventCheckoutStop);	// выкл
					}
					break;
				}
				
				if (_stateUndefined)
				{
					e.SetDt(0);
					e.SetNoLog();
				}
				else
				{
					e.SetDt(curTime);
				}
				Events::Push(e);
			}
		}
		
		_prevDriveState = curDriveState;
		_prevProtectionState = curProtectionState;
		_prevActiveController = curActiveController;
		_stateUndefined = false;
	}
};
template<class ActiveDriveControllerParams, class Events>
bool DriveEventsGenerator<ActiveDriveControllerParams, Events>::_stateUndefined = true;

template<class ActiveDriveControllerParams, class Events>
unsigned short DriveEventsGenerator<ActiveDriveControllerParams, Events>::_prevDriveState = 0;

template<class ActiveDriveControllerParams, class Events>
unsigned short DriveEventsGenerator<ActiveDriveControllerParams, Events>::_prevProtectionState = 0;


template<class ActiveDriveControllerParams, class Events>
DriveEventsGenerator<ActiveDriveControllerParams, Events>::ActiveController
DriveEventsGenerator<ActiveDriveControllerParams, Events>::_prevActiveController = 
DriveEventsGenerator<ActiveDriveControllerParams, Events>::ACUndefined;


#endif