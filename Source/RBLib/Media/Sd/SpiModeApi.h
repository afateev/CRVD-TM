
#ifndef _MEDIA_SD_SPI_MODE_API_H_
#define _MEDIA_SD_SPI_MODE_API_H_

#include "Response/Status.h"
#include "crc16.h"
#include "crc7.h"
#include "ErrorsCounterSpi.h"

namespace Rblib
{
namespace Media
{
namespace Sd
{

template<class Spi,
		class CsPort,
		unsigned char CsPin>
class SpiModeApi
{
public:

	typedef ErrorsCounterSpi ErrorsCounter;
	static const unsigned char MaxCmdRepeats = 10;

public:

	// инициализация
	static bool Init()
	{
		Select(false);
	  
		unsigned char snd = 0xff;
		unsigned char rcv;

		for(char i = 0; i < 10; i++)
			Spi::WriteWithoutInterrupts(&snd, &rcv, 1);

		return true;
	}

	// отправка команды
	static bool SendCommand(unsigned char cmd, unsigned long arg, unsigned char respType, unsigned char* resp)
	{
		Select(true);
        
		if (!WaitReady(cmd == 0 ? true : false))
			return false;

		unsigned char readBack[4];

		unsigned char crc = CrcByte(cmd, arg);
		
		cmd |= _maskCmdTransmissionBit;
		cmd &= ~_maskCmdStartBit;

		Spi::WriteWithoutInterrupts(&cmd, readBack, 1);
		
		for(char i = 0; i < 4; i++)
		{
			Spi::WriteWithoutInterrupts(((unsigned char*)&arg) + 3 - i, readBack, 1);
		}
		
		Spi::WriteWithoutInterrupts(&crc, readBack, 1);

		bool res = ReadResponse(cmd & ~_maskCmdTransmissionBit, resp);

		if (!WaitReady())
			return false;

		Select(false);

		return res;
	}

	// отправить блок данных
	static bool SendDataBlock(unsigned char* buffer, unsigned long len)
	{
		Select(true);

		DummyCycles(2); // пара циклов перед данными

		StartData();
		WriteData(buffer, len);

		// шлём CRC
		unsigned short crc = Crc16::Calc(buffer, len);
		WriteDataFromHighByte((unsigned char*)&crc, 2);

		bool res = CheckWriteStatus() && WaitBusy();

		Select(false);

		return res;
	}

	// считать блок данных
	static bool ReadDataBlock(unsigned char* buffer, unsigned long len)
	{
		Select(true);

		if (!WaitData())
			return false;

		ReadDataFromLowByte(buffer, len);

		unsigned short crc;
		ReadDataFromHighByte((unsigned char*)&crc, 2);

		Select(false);

		if (crc == Crc16::Calc(buffer, len))
			return true;

		_errorsCounter.IncError(ErrorsCounter::ReadCrc);

		return false;
	}

	// возвращает счётчики ошибок
	static const ErrorsCounter& Errors()
	{
		return _errorsCounter;
	}

protected:

	//
	// параметры в зависимости от команды
	//

	// CRC команды
	static inline unsigned char CrcByte(unsigned char cmd, unsigned long arg)
	{
		switch (cmd) // для этих команд CRC не меняется
		{
		case 0:
			return 0x95;
		case 8:
			return 0x87;
		}

		// выставляем start и transmission биты в команде, считаем CRC и добавляем end bit
		return Crc7::Calc((cmd | _maskCmdTransmissionBit) & ~_maskCmdStartBit, arg) | 1;
	}

	// полезные данные ответа без заголовка
	inline static unsigned long RespLength(unsigned char cmd)
	{
		switch (cmd)
		{
		case 8:
		case 58:
			return 4;
		case 13:
			return 1;
		case 9:
		case 10:
			return 15;
		}

		return 0;
	}

	// команды за которыми следует отдельная посылка данных
	static inline bool CidOrCsd(unsigned char cmd)
	{
		return cmd == 9 || cmd == 10;
	}

	//
	// chip select
	//

	// выбираем заданную ногой карту для обмена
	static inline void Select(bool select)
	{
		if (select)
		{
			CsPort::ClearBit(CsPin);
		}
		else
		{
			CsPort::SetBit(CsPin);
		}
	}

	//
	// приём и отправка данных
	//

	// принимаем ответ
    static bool ReadResponse(unsigned char cmd, unsigned char* resp)
    {
		if (!ReadResponseHeader(resp))
			return false;

		Response::Status* header = (Response::Status*)resp;

		if (!header->Valid() || header->Error())
		{
			IncCmdErrors(header);
			return false;
		}

		unsigned long len = RespLength(cmd);

		if (len == 0)
			return true;

		if (CidOrCsd(cmd))
		{
			if (!WaitData())
				return false;

			ReadDataFromHighByte(resp + 1, len);

			unsigned short crc;
			ReadDataFromLowByte((unsigned char *)&crc, 1); // читаем crc регистра, 8 бит
			ReadDataFromHighByte((unsigned char*)&crc, 2); // читаем crc блока данных, 16 бит
		}
		else
			ReadDataFromHighByte(resp + 1, len);

		return true;
    }

	// принимаем заголовок ответа (или это весь ответ R1)
	static bool ReadResponseHeader(unsigned char* resp)
	{
		unsigned char sendBuff = 0xff;
		unsigned char repeatCnt = 0xff;

		while (repeatCnt)
		{
			Spi::WriteWithoutInterrupts(&sendBuff, resp, 1);
			if (*resp & _responseMask)
				repeatCnt--;
			else
				return true;
		}

		_errorsCounter.IncError(ErrorsCounter::CmdResponse);

		return false;
	}

	// принимаем данные, записывая в буфер с конца
	inline static void ReadDataFromHighByte(unsigned char* dataBuffer, unsigned long length)
	{
		unsigned char snd = 0xff;

		for (int i = 0; i < length; i++)
		{
			Spi::WriteWithoutInterrupts(&snd, dataBuffer + length - 1 - i, 1);
		}
	}

	// принимаем данные, записывая в буфер с начала
	inline static void ReadDataFromLowByte(unsigned char* dataBuffer, unsigned long length)
	{
		unsigned char snd = 0xff;

		for (int i = 0; i < length; i++)
		{
			Spi::WriteWithoutInterrupts(&snd, dataBuffer + i, 1);
		}
	}

	// шлём данные, начиная со старших байтов
	inline static void WriteDataFromHighByte(unsigned char* dataBuffer, unsigned long length)
	{
		unsigned char r;

		while(length--)
		{
			Spi::WriteWithoutInterrupts(dataBuffer + length, &r, 1);
		}
	}

	// шлём данные
	inline static void WriteData(unsigned char* dataBuffer, unsigned long length)
	{
		unsigned char r = 0;

		for (unsigned long i = 0; i < length; i++)
		{
			Spi::WriteWithoutInterrupts(dataBuffer + i, &r, 1);
		}
	}

	// начало передачи, отправляем токен начала данных
	static inline void StartData()
	{
		unsigned char r = 0;
		unsigned char s = _dataStartToken;
		Spi::WriteWithoutInterrupts(&s, &r, 1);
	}
	
	// пустые посылки/чтения
	static inline void DummyCycles(unsigned long n)
	{
		unsigned char snd = 0xff;
		unsigned char rcv = 0;

		for (unsigned long i = 0; i < n; i++)
		{
			Spi::WriteWithoutInterrupts(&snd, &rcv, 1);
		}
	}

	//
	// проверка и ожидание статусов
	//

	// приняты ля данные для записи или произошла ошибка?
	static inline bool CheckWriteStatus()
	{
		unsigned char snd = 0xff;
		unsigned char rcv = 0;

		Spi::WriteWithoutInterrupts(&snd, &rcv, 1);

		if ((rcv & _dataWriteStatusMask) == _dataWriteStatusOk)
			return true;

		IncWriteErrors(rcv);

		return false;
	}

	//
	// ждём, пока карта прекратит что-либо передавать
	//
	static inline bool WaitReady()
	{
		return WaitReady(_retNumber);
	}

	static inline bool WaitReady(bool init)
	{
		return WaitReady(init ? _retNumberInit : _retNumber);
	}

	static inline bool WaitReady(unsigned short retNum)
	{
		if (WaitByte(0xff, retNum))
			return true;
		_errorsCounter.IncError(ErrorsCounter::CmdWait);
		return false; 
	}

	//
	// ждём начала передачи данных
	//
	static inline bool WaitData()
	{
		if (WaitByte(_dataStartToken))
			return true;
		_errorsCounter.IncError(ErrorsCounter::ReadWait);
		return false;
	}

	//
	// ждём, пока карта не запишет данные
	//
	static inline bool WaitBusy()
	{
		if (WaitByte(0, true))
			return true;
		_errorsCounter.IncError(ErrorsCounter::WriteWait);
		return false;
	}

	//
	// ждём определнный байт от карты
	//
	static inline bool WaitByte(unsigned char val)
	{
		return WaitByte(val, false);
	}

	static inline bool WaitByte(unsigned char val, unsigned short retNum)
	{
		return WaitByte(val, false, retNum);
	}

	static inline bool WaitByte(unsigned char val, bool allButVal)
	{
		return WaitByte(val, allButVal, _retNumber);
	}

	static bool WaitByte(unsigned char val, bool allButVal, unsigned short retNum)
	{
		unsigned char r = 0;
		unsigned char s = 0xff;

		do 
		{
			Spi::WriteWithoutInterrupts(&s, &r, 1);

			if (allButVal)
			{
				if (r != val)
					return true;
			}
			else
			{
				if (r == val)
					return true;
			}
			
		} while(--retNum);

		return false;
	}

	static void IncCmdErrors(Response::Status* status)
	{
		if (status->EraseReset())
			_errorsCounter.IncError(ErrorsCounter::CmdEraseReset);
		if (status->EraseSequenceError())
			_errorsCounter.IncError(ErrorsCounter::CmdEraseSequence);
		if (status->IllegalCommand())
			_errorsCounter.IncError(ErrorsCounter::CmdIllegalCommand);
		if (status->CrcError())
			_errorsCounter.IncError(ErrorsCounter::CmdCrc);
		if (status->AddressError())
			_errorsCounter.IncError(ErrorsCounter::CmdAddress);
		if (status->ParameterError())
			_errorsCounter.IncError(ErrorsCounter::CmdParameter);
	}

	static void IncWriteErrors(unsigned char status)
	{
		status &= _dataWriteStatusMask;
		if (status == _dataWriteStatusCrcError)
			_errorsCounter.IncError(ErrorsCounter::WriteCrc);
		if (status == _dataWriteStatusError)
			_errorsCounter.IncError(ErrorsCounter::Write);
	}

protected:

	static const unsigned short _retNumber = 50000;
	static const unsigned short _retNumberInit = 5000;

	static const unsigned char _maskCmdStartBit = 0x80;
	static const unsigned char _maskCmdTransmissionBit = 0x40;
	static const unsigned char _responseMask = 0x80;

	static const unsigned char _dataStartToken = 0xfe;

	static const unsigned char _dataWriteStatusMask = 0x1f;
	static const unsigned char _dataWriteStatusOk = 0x05;
	static const unsigned char _dataWriteStatusCrcError = 0x0b;
	static const unsigned char _dataWriteStatusError = 0x0d;

protected:

	// счётчики ошибок
	static ErrorsCounter _errorsCounter;
};

#define SPI_MODE_API_TEMPLATE_HEAD template<class Spi, class CsPort, unsigned char CsPin>
#define SPI_MODE_API_CLASS_NAME SpiModeApi<Spi, CsPort, CsPin>

SPI_MODE_API_TEMPLATE_HEAD SPI_MODE_API_CLASS_NAME::ErrorsCounter SPI_MODE_API_CLASS_NAME::_errorsCounter;

}
}
}

#endif // _MEDIA_SD_SPI_MODE_API_H_

