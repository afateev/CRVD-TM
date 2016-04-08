
#ifndef _MEDIA_SD_ERRORS_COUNTER_SPI_H_
#define _MEDIA_SD_ERRORS_COUNTER_SPI_H_

namespace Rblib
{
namespace Media
{
namespace Sd
{

class ErrorsCounterSpi
{

public:

	static const unsigned char ErrorsNum = 13;

public:

	enum
	{
		CmdResponse = 0,
		CmdEraseReset,
		CmdEraseSequence,
		CmdIllegalCommand,
		CmdCrc,
		CmdAddress,
		CmdParameter,
		CmdWait,
		WriteCrc,
		Write,
		WriteWait,
		ReadCrc,
		ReadWait
	};

public:

	ErrorsCounterSpi()
	{
		for (unsigned char i = 0; i < ErrorsNum; i++)
			_error[i] = 0;
	}

	inline void IncError(unsigned char e)
	{
		if (e >= ErrorsNum)
			return;
		_error[e]++;
	}

	inline unsigned long operator[](unsigned char e) const
	{
		if (e >= ErrorsNum)
			return 0;
		return _error[e]; 
	}

	inline bool operator==(const ErrorsCounterSpi& err)
	{
		for (unsigned char e = 0; e < ErrorsNum; e++)
		{
			if (_error[e] != err._error[e])
				return false;
		}
		return true;
	}

protected:

	unsigned long _error[ErrorsNum];
};

}
}
}

#endif // _MEDIA_SD_ERRORS_COUNTER_SPI_H_

