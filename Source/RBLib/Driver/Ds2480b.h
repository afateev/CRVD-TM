#ifndef DRIVER_DS2480B_H
#define DRIVER_DS2480B_H

#include "../Utils/CallbackWrapper.h"
#include "Ds2480b/Commands.h"
#include "Ds2480b/Scenario.h"
#include "Ds2480b/ScenarioDetect.h"
#include "Ds2480b/ScenarioSearchRom.h"
#include "Ds2480b/ScenarioSkipRom.h"
#include "Ds2480b/ScenarioMatchRom.h"

namespace Rblib
{
namespace Driver
{
	template
		<
			class InterfaceType
		>
	class Ds2480b
	{
	public:
		typedef InterfaceType Interface;
		
		enum State
		{
			StateNotInited,
			StateIdle,
			StateDetect,
			StateSearchRom,
			StateSkipRom,
			StateMatchRom
		};
	protected:
		State _state;
		Ds2480bImplementation::ScenarioDetect _detect;
		Ds2480bImplementation::ScenarioSearchRom _searchRom;
		Ds2480bImplementation::ScenarioSkipRom _skipRom;
		Ds2480bImplementation::ScenarioMatchRom _matchRom;
		
		Ds2480bImplementation::Scenario *_currentScenario;
		
		bool _detectPending;
		bool _detected;
		bool _searchRomPending;
		bool _skipRomPending;
		unsigned char _skipRomCmd;
		unsigned char _skipRomData[128];
		unsigned int _skipRomDataCount;
		bool _matchRomPending;
		unsigned char _matchRomId[8];
		unsigned char _matchRomCmd;
		unsigned char _matchRomData[128];
		unsigned int _matchRomDataCount;
	public:
		CallbackWrapper<unsigned char *> OnDeviceFound;
		CallbackWrapper<unsigned char *, int> OnSkipRomResultReaded;
		CallbackWrapper<unsigned char *, int> OnMatchRomResultReaded;
	public:
		Ds2480b()
		{
			_state = StateNotInited;
			_currentScenario = 0;
			_detectPending = false;
			_detected = false;
			_searchRomPending = false;
			_skipRomPending = false;
			_matchRomPending = false;
		}
		
		void Init()
		{
			if (_state != StateNotInited)
			{
				return;
			}
			
			Interface::OnWriteCompleteCallback.Set(OnWriteCompleteCallback, this);
			Interface::OnReadCallback.Set(OnReadCallback, this);
			
			_detect.Write = Interface::Write;
			_detect.SendBreak = Interface::SendBreak;
			_detectPending = true;
			_detected = false;
			
			_searchRom.Write = Interface::Write;
			_searchRom.OnDeviceFound.Set(OnDeviceFoundCallback, this);
			
			_skipRom.Write = Interface::Write;
			_skipRom.OnResultReaded.Set(OnSkipRomResultReadedCallback, this);
			
			_matchRom.Write = Interface::Write;
			_matchRom.OnResultReaded.Set(OnMatchRomResultReadedCallback, this); 
			
			SetState(StateIdle);
		}
		
		void Tick(unsigned int tickFrequency)
		{
			if (_currentScenario)
			{
				_currentScenario->Tick(tickFrequency);
			}
			
			switch(_state)
			{
			case StateIdle:
				{
					if (_detectPending)
					{
						SetState(StateDetect);
						_detectPending = false;
						_detected = false;
						_currentScenario = &_detect;
						if (_currentScenario)
						{
							_currentScenario->Start();
						}
						
						break;
					}
					
					if (_searchRomPending)
					{
						SetState(StateSearchRom);
						_searchRomPending = false;
						_currentScenario = &_searchRom;
						if (_currentScenario)
						{
							_currentScenario->Reset();
							_currentScenario->Start();
						}
						
						break;
					}
					
					if (_skipRomPending)
					{
						SetState(StateSkipRom);
						_skipRomPending = false;
						_currentScenario = &_skipRom;
						if (_currentScenario)
						{
							_currentScenario->Reset();
							_currentScenario->Start(_skipRomCmd, _skipRomData, _skipRomDataCount);
						}
						break;
					}
					
					if (_matchRomPending)
					{
						SetState(StateMatchRom);
						_matchRomPending = false;
						_currentScenario = &_matchRom;
						if (_currentScenario)
						{
							_currentScenario->Reset();
							_currentScenario->Start(_matchRomId, _matchRomCmd, _matchRomData, _matchRomDataCount);
						}
						break;
					}
				}
				break;
			case StateDetect:
				{
					bool result = false;
					if (_detect.Done(result))
					{
						SetState(StateIdle);
						_detected = result;
					}
				}
				break;
			case StateSkipRom:
			case StateSearchRom:
			case StateMatchRom:
				{
					bool result = false;
					if (_currentScenario)
					{
						if (_currentScenario->Done(result))
						{
							SetState(StateIdle);
						}
					}
					else
					{
						SetState(StateIdle);
					}
				}
				break;
			}
		}
		
		bool Detected()
		{
			return _detected;
		}
		
		void SearchRom()
		{
			_searchRomPending = true;
		}
		
		void SkipRom(unsigned char cmd, unsigned char *data, unsigned char dataCount)
		{
			_skipRomCmd = cmd;
			
			_skipRomDataCount = dataCount;
			if (_skipRomDataCount > sizeof(_skipRomData))
			{
				_skipRomDataCount = sizeof(_skipRomData);
			}
			
			for (int i = 0; i < _skipRomDataCount; i++)
			{
				_skipRomData[i] = data[i];
			}
			
			_skipRomPending = true;
		}
		
		void MatchRom(unsigned char *romId, unsigned char cmd, unsigned char *data, unsigned char dataCount)
		{
			for (int i = 0; i < sizeof(_matchRomId); i++)
			{
				_matchRomId[i] = romId[i];
			}
			
			_matchRomCmd = cmd;
			
			_matchRomDataCount = dataCount;
			if (_matchRomDataCount > sizeof(_matchRomData))
			{
				_matchRomDataCount = sizeof(_matchRomData);
			}
			
			for (int i = 0; i < _matchRomDataCount; i++)
			{
				_matchRomData[i] = data[i];
			}
			
			_matchRomPending = true;
		}
	protected:
		void SetState(State newState)
		{
			_state = newState;
		}
		
		void Write(void *data, unsigned int len)
		{
			Interface::Write((unsigned char *)data, len);
		}
		
		void OnWriteComplete()
		{
			if (_currentScenario)
			{
				_currentScenario->OnWriteComplete();
			}
		}
		
		static void OnWriteCompleteCallback(void *callbackParam)
		{
			Ds2480b *im = (Ds2480b *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->OnWriteComplete();
		}
		
		void OnRead(unsigned char data)
		{
			if (_currentScenario)
			{
				_currentScenario->OnRx(data);
			}
		}
		
		static void OnReadCallback(void *callbackParam, unsigned char data)
		{
			Ds2480b *im = (Ds2480b *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->OnRead(data);
		}
		
		static void OnDeviceFoundCallback(void *callbackParam, unsigned char *deviceId)
		{
			Ds2480b *im = (Ds2480b *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->OnDeviceFound(deviceId);
		}
		
		static void OnSkipRomResultReadedCallback(void *callbackParam, unsigned char *data, int count)
		{
			Ds2480b *im = (Ds2480b *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->OnSkipRomResultReaded(data, count);
		}
		
		static void OnMatchRomResultReadedCallback(void *callbackParam, unsigned char *data, int count)
		{
			Ds2480b *im = (Ds2480b *)callbackParam;
			if (!im)
			{
				return;
			}
			
			im->OnMatchRomResultReaded(data, count);
		}
	};
}
}

#endif