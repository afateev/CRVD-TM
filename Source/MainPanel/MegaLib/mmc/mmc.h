////////////////////////////////////////////////////////////////////////////////
//
//
//	Fateev A.I.		31/01/2012
////////////////////////////////////////////////////////////////////////////////

#ifndef MMC_H
#define MMC_H

template<class Spi>
class MMCProtocol
{
protected:
	enum Command
	{
		CmdGoIdleState = 0,
		CmdSendOpCond = 1,
		CmdSendInterfaceCondition = 8,
		CmdSendCSD = 9,
		CmdSendCID = 10,
		CmdSendStatus = 13,
		CmdReadSingleBlock = 17,
		CmdWriteSingleBlock = 24,
		CmdApplication = 55,
		CmdReadOcr = 58
	};
	
	enum ACommand
	{
		ACmdSendOperationCondition = 41
	};
	
	struct ResponseType1
	{
		unsigned char InIdleState : 1;
		unsigned char EraseReset : 1;
		unsigned char IlligalCommand : 1;
		unsigned char CommandCrcError : 1;
		unsigned char EraseSequenseError : 1;
		unsigned char AddressError : 1;
		unsigned char ParameterError : 1;
		unsigned char StartBit : 1;
		
		bool Valid()
		{
			return StartBit == 0;
		}
		
		bool HasError()
		{
			return IlligalCommand || CommandCrcError || EraseSequenseError || AddressError || ParameterError;
		}
	};
	
	struct ResponseType2
	{
		ResponseType1 R1;
		unsigned char CardIsLocked : 1;
		unsigned char WpEraseSkip : 1;
		unsigned char Error : 1;
		unsigned char CCError : 1;
		unsigned char CardEccFailed : 1;
		unsigned char WPViolation : 1;
		unsigned char EraseParam : 1;
		unsigned char OutOfRangeCsdOverwrite : 1;
		
		bool Valid()
		{
			return R1.StartBit == 0;
		}
		
		bool HasError()
		{
			return R1.IlligalCommand || R1.CommandCrcError || R1.EraseSequenseError || R1.AddressError || R1.ParameterError 
				|| CardIsLocked || WpEraseSkip || Error || CCError || CardEccFailed || WPViolation || EraseParam || OutOfRangeCsdOverwrite;
		}
	};
	
	struct ResponseType3
	{
		ResponseType1 R1;
		/*
		unsigned char : 8;
		unsigned char : 7;
		unsigned char v2_7_2_8 : 1;
		unsigned char v2_8_2_9 : 1;
		unsigned char v2_9_3_0 : 1;
		unsigned char v3_0_3_1 : 1;
		unsigned char v3_1_3_2 : 1;
		unsigned char v3_2_3_3 : 1;
		unsigned char v3_3_3_4 : 1;
		unsigned char v3_4_3_5 : 1;
		unsigned char v3_5_3_6 : 1;
		unsigned char : 6;
		unsigned char CCS : 1;
		unsigned char Busy: 1;*/
		
		unsigned char : 6;
		unsigned char CCS : 1;
		unsigned char Busy: 1;
		
		unsigned char v2_8_2_9 : 1;
		unsigned char v2_9_3_0 : 1;
		unsigned char v3_0_3_1 : 1;
		unsigned char v3_1_3_2 : 1;
		unsigned char v3_2_3_3 : 1;
		unsigned char v3_3_3_4 : 1;
		unsigned char v3_4_3_5 : 1;
		unsigned char v3_5_3_6 : 1;
		
		
		unsigned char : 7;
		unsigned char v2_7_2_8 : 1;
		
		unsigned char : 8;
		
		/*
		unsigned char Busy: 1;
		unsigned char CCS : 1;
		unsigned char : 6;
		unsigned char v3_5_3_6 : 1;
		unsigned char v3_4_3_5 : 1;
		unsigned char v3_3_3_4 : 1;
		unsigned char v3_2_3_3 : 1;
		unsigned char v3_1_3_2 : 1;
		unsigned char v3_0_3_1 : 1;
		unsigned char v2_9_3_0 : 1;
		unsigned char v2_8_2_9 : 1;
		unsigned char v2_7_2_8 : 1;
		unsigned char : 7;
		unsigned char : 8;
		*/
	};
	
	struct ResponseType7
	{
		ResponseType1 R1;
		unsigned char reserved[4];
	};
	
	struct CSDStruct
	{
		unsigned char CSDStructure;
		unsigned char SystemSpecificationVersion;
		unsigned char DataReadAccessTime1;
		unsigned char DataReadAccessTime2;
		unsigned char MaxBusClockFrequency;
		unsigned short CardCommandClasses;
		unsigned char MaxReadDataBlockLength;
		unsigned char PartialBlocksForReadAllowed;
		unsigned char WriteBlockMisalignment;
		unsigned char ReadBlockMisalignment;
		unsigned char DSRImplemented;
		unsigned short DeviceSize;
		unsigned char MaxReadCurrentVDDmin;
		unsigned char MaxReadCurrentVDDmax;
		unsigned char MaxWriteCurrentVDDmin;
		unsigned char MaxWriteCurrentVDDmax;
		unsigned char DeviceSizeMultiplier;
		unsigned char EraseGroupSize;
		unsigned char EraseGroupSizeMultiplier;
		unsigned char WriteProtectGroupSize;
		unsigned char WriteProtectGroupEnable;
		unsigned char ManufacturerDefaultECC;
		unsigned char WriteSpeedFactor;
		unsigned char MaxWriteDataBlockLength;
		unsigned char PartialBlocksForWriteAllowed;
		unsigned char ContentProtectionApplication;
		unsigned char FileFormatGroup;
		unsigned char CopyFlag;
		unsigned char PermanentWriteProtection;
		unsigned char TmpWriteProtection;
		unsigned char FileFormat;
		unsigned char EccCode;
		unsigned char CRC;
		unsigned char NotUsed;
		
		unsigned int DeviceSizeV2;
		
		void Parse(unsigned char *data)
		{
			unsigned int tmp[4];
			char k = 0;
			for (char i = 0; i < 4; i ++)
			{
				tmp[i] = 0;
				for (char j = 0; j < 4; j++)
				{
					tmp[i] <<= 8;
					tmp[i] |= data[k];
					k++;
				}
			}
			
			CSDStructure = UnstuffBits(tmp, 126, 2);
			SystemSpecificationVersion = UnstuffBits(tmp, 122, 4);
			DataReadAccessTime1 = UnstuffBits(tmp, 112, 8);
			DataReadAccessTime2 = UnstuffBits(tmp, 104, 8);
			MaxBusClockFrequency = UnstuffBits(tmp, 96, 8);
			CardCommandClasses = UnstuffBits(tmp, 84, 12);
			MaxReadDataBlockLength = UnstuffBits(tmp, 80, 4);
			PartialBlocksForReadAllowed = UnstuffBits(tmp, 79, 1);
			WriteBlockMisalignment = UnstuffBits(tmp, 78, 1);
			ReadBlockMisalignment = UnstuffBits(tmp, 77, 1);
			DSRImplemented = UnstuffBits(tmp, 76, 1);
			DeviceSize = UnstuffBits(tmp, 62, 12);
			DeviceSizeV2 = UnstuffBits(tmp, 48, 22);
			MaxReadCurrentVDDmin = UnstuffBits(tmp, 59, 3);
			MaxReadCurrentVDDmax = UnstuffBits(tmp, 56, 3);
			MaxWriteCurrentVDDmin = UnstuffBits(tmp, 53, 3);
			MaxWriteCurrentVDDmax = UnstuffBits(tmp, 50, 3);
			DeviceSizeMultiplier = UnstuffBits(tmp, 47, 3);
			EraseGroupSize = UnstuffBits(tmp, 42, 5);
			EraseGroupSizeMultiplier = UnstuffBits(tmp, 37, 5);
			WriteProtectGroupSize = UnstuffBits(tmp, 32, 5);
			WriteProtectGroupEnable = UnstuffBits(tmp, 31, 1);
			ManufacturerDefaultECC = UnstuffBits(tmp, 29, 2);
			WriteSpeedFactor = UnstuffBits(tmp, 26, 3);
			MaxWriteDataBlockLength = UnstuffBits(tmp, 22, 4);
			PartialBlocksForWriteAllowed = UnstuffBits(tmp, 21, 1);
			ContentProtectionApplication = UnstuffBits(tmp, 16, 1);
			FileFormatGroup = UnstuffBits(tmp, 15, 1);
			CopyFlag = UnstuffBits(tmp, 14, 1);
			PermanentWriteProtection = UnstuffBits(tmp, 13, 1);
			TmpWriteProtection = UnstuffBits(tmp, 12, 1);
			FileFormat = UnstuffBits(tmp, 10, 2);
			EccCode = UnstuffBits(tmp, 8, 2);
			CRC = UnstuffBits(tmp, 7, 1);
			NotUsed = UnstuffBits(tmp, 0, 1);
		}
		
		bool Valid()
		{
			return NotUsed == 1;
		}
	};
	
	struct CIDStruct
	{
		unsigned char ManufactureId;
		unsigned char OemApplicationId[2];
		unsigned char ProductName[6];
		unsigned char ProductRevision;
		unsigned char ProductSerialNumber[4];
		unsigned char ManufacturingDate;
		unsigned char NotUsed : 1;
		unsigned char Crc7 : 7;
		
		bool Valid()
		{
			return NotUsed == 1;
		}
	};
	
	CSDStruct CSD;
	CIDStruct CID;
public:
	void Init()
	{
		Spi::Init();
	}
	
	bool Reset()
	{
		memset(&CID, 0, sizeof(CID));
		memset(&CSD, 0, sizeof(CSD));
		
		Spi::Select(true);
		Dummy(2048);
		SendCommand(CmdGoIdleState, 0);
		bool result = false;
		ResponseType1 response;
		if (GetResponse((unsigned char *)&response, 1))
		{
			if (response.Valid())
			{
				result = response.InIdleState;
				/*
				if (result)
				{
					result = 0;
					for (unsigned short i = 0; i < 100; i++)
					{
						SendCommand(CmdSendOpCond, 0);
						if (GetResponse((unsigned char *)&response, 1))
						{
							if (response.Valid())
							{
								result = response.InIdleState == 0;
								if (result)
									break;
							}
						}
					}
				}
				*/
			}
		}
		
		Spi::Select(false);
		return result;
	}
	
	bool SendInterfaceCondition(bool &illigalCommand)
	{
		Spi::Select(true);
		Dummy(8);
		
		bool result = false;
		ResponseType7 response;
		
		SendCommand(CmdSendInterfaceCondition, 0x01AA, 0x87);
				
		if (GetResponse((unsigned char *)&response, sizeof(response)))
		{
			if (response.R1.Valid())
			{
				illigalCommand = response.R1.IlligalCommand;
				result = true;
			}
		}
		
		Spi::Select(false);
		return result;
	}
	
	bool SendOperationCondition(bool hcs, bool &ready)
	{
		Spi::Select(true);
		Dummy(8);
		
		bool result = false;
		ResponseType1 response;
		
		if (SendCommandA(ACmdSendOperationCondition, hcs ? (1 << 30) : 0))
		{
			if (GetResponse((unsigned char *)&response, sizeof(response)))
			{
				if (response.Valid())
				{
					if (!response.HasError())
					{
						ready = response.InIdleState == 0;
						result = true;
					}
				}
			}
		}
		
		Spi::Select(false);
		return result;
	}
	
	bool SendStatus(bool &error, bool &idle)
	{
		Spi::Select(true);
		
		bool result = false;
		ResponseType2 response;
		
		SendCommand(CmdSendStatus, 0);
		if (GetResponse((unsigned char *)&response, sizeof(response)))
		{
			if (response.Valid())
			{
				error = response.HasError();
				idle = response.R1.InIdleState;
				if (error)
				{
					result = true;
				}
				if (error)
				{
					result = true;
				}
				// статус вычитлс€, считаем метод успешно выполненным, нзависимоот состо€ни€
				result = true;
			}
		}
		else
		{
			result = false;
		}
		
		Spi::Select(false);
		return result;
	}
	
	bool ReadOcr(bool &ccs)
	{
		Spi::Select(true);
		
		bool result = false;
		ResponseType3 response;
		
		SendCommand(CmdReadOcr, 0);
		if (GetResponse((unsigned char *)&response, sizeof(response)))
		{
			if (response.R1.Valid())
			{
				if (!response.R1.HasError())
				{
					ccs = response.CCS;
					result = true;
				}
			}
		}
		
		Spi::Select(false);
		return result;
	}
	
	bool CheckReady()
	{
		Dummy(10);
		
		Spi::Select(true);
		Dummy(8);
		
		bool result = false;
		ResponseType1 response;
		
		for (unsigned short i = 0; i < 100; i++)
		{
			SendCommand(CmdSendOpCond, 0);
			if (GetResponse((unsigned char *)&response, 1))
			{
				if (response.Valid())
				{
					result = response.InIdleState == 0;
					if (result)
						break;
				}
			}
		}
		
		Spi::Select(false);
		return result;
	}
	
	bool ReadCID()
	{
		Spi::Select(true);
		Dummy(8);
		
		bool result = false;
		ResponseType1 response;
		
		SendCommand(CmdSendCID, 0);
				
		if (GetResponse((unsigned char *)&response, sizeof(response)))
		{
			if (response.Valid())
			{
				if (!response.HasError())
				{
					unsigned char data[sizeof(CID)];
					unsigned char error;
					if (GetData(0xFE, &error, data, sizeof(data)))
					{
						memcpy(&CID, data, sizeof(data));
						result = true;
					}
				}
			}
		}
		
		Spi::Select(false);
		return result;
	}
	
	bool ReadCSD()
	{
		Spi::Select(true);
		Dummy(8);
		
		bool result = false;
		ResponseType1 response;
		
		SendCommand(CmdSendCSD, 0);
				
		if (GetResponse((unsigned char *)&response, sizeof(response)))
		{
			if (response.Valid())
			{
				if (!response.HasError())
				{
					unsigned char data[16];
					unsigned char error;
					if (GetData(0xFE, &error, data, sizeof(data)))
					{
						CSD.Parse(data);
						result = true;
					}
				}
			}
		}
		
		Spi::Select(false);
		return result;
	}
	
	bool ReadBlock(unsigned int address, unsigned char *data, unsigned int count)
	{
		Spi::Select(true);
		Dummy(8);
		
		bool result = false;
		ResponseType1 response;
		
		SendCommand(CmdReadSingleBlock, address);
				
		if (GetResponse((unsigned char *)&response, sizeof(response)))
		{
			if (response.Valid())
			{
				if (!response.HasError())
				{
					unsigned char error;
					if (GetData(0xFE, &error, data, count))
					{
						result = true;
					}
				}
			}
		}
		
		Spi::Select(false);
		return result;
	}
	
	bool WriteBlock(unsigned int address, unsigned char *data, unsigned int count)
	{
		Spi::Select(true);
	
		unsigned char rx = 0;
		for (int i = 0; i < 0xFFF; i++)
		{
			rx = Spi::WriteByte(0xFF);
			if (rx == 0xFF)
				break;
		}
		
		bool result = false;
		ResponseType1 response;
		
		SendCommand(CmdWriteSingleBlock, address);
				
		if (GetResponse((unsigned char *)&response, sizeof(response)))
		{
			if (response.Valid())
			{
				if (!response.HasError())
				{
					unsigned char error;
					if (SendData(0x05, &error, data, count))
					{
						// ожидаем заверщение операции
						for (unsigned int i = 0; i < 1000000; i++)
						{
							rx = Spi::WriteByte(0xFF);
							if (rx != 0x00)
								break;
						}
						
						if (rx != 0x00)
						{
							result = true;
						}
						else
						{
							result = false;
						}
					}
					else
					{
						result = false;
					}
				}
				else
				{
					result = false;
				}
			}
			else
			{
				result = false;
			}
		}
		else
		{
			result = false;
		}
		
		Spi::Select(false);
		return result;
	}
	
	unsigned int GetBlockSize()
	{
		unsigned int res = 0;
		if (CSD.Valid())
		{
			res = 1;
			res <<= (CSD.MaxReadDataBlockLength);
		}
		return res;
	}
	
	unsigned int GetBlockCount()
	{
		unsigned int res = 0;
		if (CSD.Valid())
		{
			if (CSD.CSDStructure == 0)
			{
				unsigned int mul = 1;
				mul <<= (CSD.DeviceSizeMultiplier + 2);
				res = (CSD.DeviceSize + 1) * mul;
			}
			if (CSD.CSDStructure == 1)
			{
				res = (CSD.DeviceSizeV2 + 1);
				res *= 1024;
			}
		}
		return res;
	}
	
	unsigned int GetSize()
	{
		unsigned int res = GetBlockSize() * GetBlockCount();
		return res;
	}
	
protected:
	void Dummy(unsigned int count)
	{
		for (unsigned int i = 0; i < count; i++)
			Spi::WriteByte(0xFF);
	}
	
	void SendCommand(Command cmd, unsigned int arg, unsigned char crc = 0x95)
	{
		unsigned char tx = 0;
		unsigned char tmp[4];
		tx = cmd & 0x3F;	// command index
		tx |= 0x40; 		// transmission bit
		Spi::WriteByte(tx);
		memcpy(tmp, &arg, 4);
		Spi::WriteByte(tmp[3]);
		Spi::WriteByte(tmp[2]);
		Spi::WriteByte(tmp[1]);
		Spi::WriteByte(tmp[0]);
		Spi::WriteByte(crc);
	}
	
	bool SendCommandA(ACommand acmd, unsigned int arg)
	{
		bool result = false;
		
		ResponseType1 response;
		SendCommand(CmdApplication, 0);
		if (GetResponse((unsigned char *)&response, sizeof(response)))
		{
			if (response.Valid())
			{
				if (!response.HasError())
				{
					SendCommand((Command)acmd, arg);
					result = true;
				}
			}
		}
		
		return result;
	}
	
	bool GetResponse(unsigned char *response, unsigned int len)
	{
		for (unsigned short i = 0; i < 0xFF; i++)
		{
			response[0] = Spi::WriteByte(0xFF);
			if ((response[0] & (1 << 7)) == 0)
			{
				for (unsigned int j = 1; j < len; j++)
					response[j] = Spi::WriteByte(0xFF);
				return true;
			}
		}
		return false;
	}
	
	bool GetData(unsigned char token, unsigned char *errorToken, unsigned char *dst, unsigned int len)
	{
		unsigned char rx = 0;
		for (unsigned short i = 0; i < 0xFFF; i++)
		{
			rx = Spi::WriteByte(0xFF);
			if (rx != 0xFF)
				break;
		}
		
		if (rx != token)
		{
			if (errorToken != 0)
				*errorToken = rx;
			return false;
		}
		
		for (unsigned int i = 0; i < len; i++)
		{
			dst[i] = Spi::WriteByte(0xFF);
		}
		
		// CRC пропускаем
		Spi::WriteByte(0xFF);
		Spi::WriteByte(0xFF);
		
		return true;
	}
	
	bool SendData(unsigned char token, unsigned char *errorToken, unsigned char *src, unsigned int len)
	{
		Spi::WriteByte(0xFF);
		
		unsigned char rx = 0;
		
		Spi::WriteByte(0xFE);
		
		for (unsigned int i = 0; i < len; i++)
		{
			Spi::WriteByte(src[i]);
		}
		
		// CRC пропускаем
		Spi::WriteByte(0xFF);
		Spi::WriteByte(0xFF);
		
		rx = Spi::WriteByte(0xFF);
		
		bool res = true;
		
		if ((rx & 0x0F) != (token & 0x0F))
		{
			if (errorToken != 0)
				*errorToken = rx;
			res = false;
		}
		
		return res;
	}
	
	static unsigned int UnstuffBits(unsigned int *data, unsigned int startBit, unsigned int sizeInBits)
	{
		unsigned int size = sizeInBits;
		unsigned int mask = (size < 32 ? 1 << size : 0) - 1;
		unsigned int off = 3 - ((startBit) / 32);
		unsigned int shft = (startBit) & 31;
		unsigned int res;
		
		res = data[off] >> shft;
		if (size + shft > 32)
			res |= data[off-1] << ((32 - shft) % 32);
		
		return res & mask;
	}
};

template<class Spi>
class MMC
{
protected:
	static MMCProtocol<Spi> _card;
	
	enum State
	{
		StateReset,
		StateSendInterfaceCondition,
		StateSendOpCond,
		StateInit,
		StateReadInfo,
		StateIdle
	};
	
	State _state;
	bool _sdDetected;
	bool _sdVer1;
	bool _sdHcDetected;
public:
	void Init()
	{
		_card.Init();
		_state = StateReset;
		_sdDetected = false;
		_sdVer1 = false;
		_sdHcDetected = false;
	}
	
	void Run()
	{
		switch(_state)
		{
		case StateReset:
			{
				_sdDetected = false;
				_sdVer1 = false;
				_sdHcDetected = false;
				if (_card.Reset())
					_state = StateSendInterfaceCondition;
			}
			break;
		case StateSendInterfaceCondition:
			{
				bool illigalCommand = false;
				if (_card.SendInterfaceCondition(illigalCommand))
				{
					_sdVer1 = illigalCommand;
					_state = StateSendOpCond;
				}
				else
				{
					_state = StateReset;
				}
			}
			break;
		// »нициализаци€ SD / SDHC
		case StateSendOpCond:
			{
				bool hcs = !_sdVer1;
				bool ready = false;
				if (_card.SendOperationCondition(hcs, ready))
				{
					if (ready)
					{
						_sdDetected = true;
						_state = StateReadInfo;
					}
				}
				else
				{
					_state = StateInit;
				}
			}
			break;
		// »нициализаци€ самой старой MMC
		case StateInit:
			{
				if (_card.CheckReady())
					_state = StateReadInfo;
				else
					_state = StateReset;
			}
			break;
		case StateReadInfo:
			{
				_card.ReadCID();
				_card.ReadCSD();
				bool ccs = false;
				if (_card.ReadOcr(ccs))
				{
					_sdHcDetected = ccs;
				}
				
				_state = StateIdle;
			}
			break;
		case StateIdle:
			{
				if (_sdDetected)
				{
					// SD / SDHC
					bool error = false;
					bool idle = false;
					if (!_card.SendStatus(error, idle))
					{
						_state = StateReset;
						break;
					}
					if (error)
					{
						_state = StateReset;
						break;
					}
					
					if (idle)
					{
						_state = StateSendInterfaceCondition;
						break;
					}
				}
				else
				{
					// MMC
					if (!_card.CheckReady())
						_state = StateReset;
				}
			}
			break;
		}
	}
	
	bool Ready()
	{
		return _state == StateIdle;
	}
	
	bool ReadBlock(unsigned long long address, unsigned char *data, unsigned int count)
	{
		if (!Ready())
			return 0;
		
		unsigned long long addr = address;
		if (_sdHcDetected)
		{
			addr /= 512;
		}
		return _card.ReadBlock(addr, data, count);
	}
	
	bool WriteBlock(unsigned long long address, unsigned char *data, unsigned int count)
	{
		if (!Ready())
			return 0;
		
		unsigned long long addr = address;
		if (_sdHcDetected)
		{
			addr /= 512;
		}
		return _card.WriteBlock(addr, data, count);
	}
	
	unsigned int GetBlockSize()
	{
		return _card.GetBlockSize();
	}
	
	unsigned int GetBlockCount()
	{
		return _card.GetBlockCount();
	}
};

template<class Spi>
MMCProtocol<Spi> MMC<Spi>::_card;

#endif
