#ifndef DRIVER_DS2480B_SCENARIOSEARCHROM_H
#define DRIVER_DS2480B_SCENARIOSEARCHROM_H

#include "../Utils/CallbackWrapper.h"
#include "Commands.h"
#include "Scenario.h"

namespace Rblib
{
namespace Driver
{
namespace Ds2480bImplementation
{
	class ScenarioSearchRom : public Scenario
	{
	public:
		CallbackWrapper<unsigned char *> OnDeviceFound;
	public:
		enum State
		{
			StateIdle,
			StateReset,
			StateWaitResetResponse,
			StateSendSearchSequence,
			StateWaitSearchResponse,
			StateDone,
			StateError
		};
	protected:
		State _state;
		unsigned char _buffer[128];
		unsigned char _searchData[16];
	public:
		ScenarioSearchRom()
		{
			
		}
		
		virtual void Reset()
		{
			Scenario::Reset();
			_state = StateIdle;
		}
		
		virtual void Start()
		{
			Scenario::Start();
			if (_state != StateIdle)
			{
				return;
			}
			
			ResetSearchData();
			_state = StateReset;
		}
		
		virtual void Tick(unsigned int tickFrequency)
		{
			Scenario::Tick(tickFrequency);
			
			switch(_state)
			{
			case StateReset:
				{
					_state = StateWaitResetResponse;
					WaitInput(1);
					// 0.5 сек таймаут ожидания ответа
					StartTimeout(tickFrequency * 0.5);
					
					CommandReset cmd;
					Write(&cmd, sizeof(cmd));
				}
				break;
			case StateWaitResetResponse:
				{
					if (InputDone())
					{
						CommandResetResponse response(_inputBuffer[0]);
						if (response.Valid())
						{
							if (response.Presence == CommandResetResponse::PresenceCodePresence)
							{
								_state = StateSendSearchSequence;
							}
							else
							{
								_state = StateDone;
							}
						}
						else
						{
							_state = StateError;
						}
					}
					else
					{
						if (Timeout())
						{
							_state = StateError;
						}
					}
				}
				break;
			case StateSendSearchSequence:
				{
					int count = 0;
					_buffer[count] = CommandSetDataMode;
					count++;
					_buffer[count] = 0xF0;	// SearchCommand
					count++;
					_buffer[count] = CommandSetCommandMode;
					count++;
					
					CommandSearchAcceleratorControl searchAcceleratorControl;
					searchAcceleratorControl.On = 1;
					_buffer[count] = *((unsigned char *)&searchAcceleratorControl);
					count++;
					
					_buffer[count] = CommandSetDataMode;
					count++;
					
					count += Stuff(_searchData, &_buffer[count], sizeof(_searchData));
					
					_buffer[count] = CommandSetCommandMode;
					count++;
					
					searchAcceleratorControl.On = 0;
					_buffer[count] = *((unsigned char *)&searchAcceleratorControl);
					count++;
					
					_state = StateWaitSearchResponse;
					WaitInput(17);
					// 1 сек таймаут ожидания ответа
					StartTimeout(tickFrequency * 1);
					
					WaitWrite();
					Write((void *)_buffer, count);
				}
				break;
			case StateWaitSearchResponse:
				{
					if (WriteComplete() && InputDone())
					{
						bool result = true;
						result &= _inputBuffer[0] == 0xF0;
						
						if (result)
						{
							unsigned char r[8];
							unsigned char d[8];
							int lastDiscrepancy = -1;
							result &= ParseResponse(&_inputBuffer[1], r, d, lastDiscrepancy);
							if (result)
							{
								OnDeviceFound(r);
								if (lastDiscrepancy < 0)
								{
									_state = StateDone;
								}
								else
								{
									BuildSearchData(_searchData, r, lastDiscrepancy);
									_state = StateReset;
								}
							}
							else
							{
								_state = StateError;
							}
						}
						else
						{
							_state = StateError;
						}
					}
					else
					{
						if (Timeout())
						{
							_state = StateError;
						}
					}
				}
				break;
			}
		}
		
		virtual bool Done(bool &result)
		{
			result = _state == StateDone;
			return _state == StateDone || _state == StateError;
		}
		
	protected:
		void ResetSearchData()
		{
			for (int i = 0; i < sizeof(_searchData); i++)
			{
				_searchData[i] = 0;
			}
		}
		
		bool ParseResponse(unsigned char *response, unsigned char *r, unsigned char *d, int &lastDiscrepancy)
		{
			if (response == 0 || r == 0 || d == 0)
			{
				return false;
			}
			
			bool res = true;
			
			static const int count = 16;
			for (int i = 0; i < count / 2; i++)
			{
				r[i] = 0;
				d[i] = 0;
			}
			
			int bitIndex = 0;
			lastDiscrepancy = -1;
			for (int i = 0; i < count; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					unsigned char field = (response[i] >> (j * 2)) & 0x3;
					unsigned char dBit = field & 1;
					unsigned char rBit = (field >> 1) & 1;
					r[bitIndex / 8] |= rBit << (bitIndex % 8);
					d[bitIndex / 8] |= dBit << (bitIndex % 8);
					
					if (dBit == 1 && rBit == 0)
					{
						lastDiscrepancy = bitIndex;
					}
					
					if (bitIndex == 63)
					{
						if (dBit == 1 && rBit == 1)
						{
							res = false;
						}
					}
					
					bitIndex++;
				}
			}
			
			return res;
		}
		
		void BuildSearchData(unsigned char *searchData, unsigned char *r, int lastDiscrepancy)
		{
			ResetSearchData();
			
			if (searchData == 0 || r == 0)
			{
				return ;
			}
			
			for (int i = 0; i < 64; i++)
			{
				unsigned char bit = 0;
				if (i < lastDiscrepancy)
				{
					bit = (r[i / 8] >> (i % 8)) & 1;
				}
				
				if (i == lastDiscrepancy)
				{
					bit = 1;
				}
				
				if (bit)
				{
					SetBit(searchData, i * 2 + 1);
				}
			}
		}
		
		void SetBitImpl(unsigned char *val, unsigned char bit)
		{
			*val |= 1 << bit;
		}
		
		void SetBit(unsigned char *array, unsigned int bit)
		{
			unsigned char *byte = &array[bit / 8];
			SetBitImpl(byte, bit % 8);
		}
		
		void ClearBitImpl(unsigned char *val, unsigned char bit)
		{
			*val &= ~(1 << bit);
		}
		
		void ClearBit(unsigned char *array, unsigned int bit)
		{
			unsigned char *byte = &array[bit / 8];
			ClearBitImpl(byte, bit % 8);
		}
	};
}
}
}

#endif