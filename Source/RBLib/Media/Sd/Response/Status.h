
#ifndef _MEDIA_SD_RESPONSE_STATUS_H_
#define _MEDIA_SD_RESPONSE_STATUS_H_

namespace Rblib
{
namespace Media
{
namespace Sd
{
namespace Response
{
//
// статус команды
//

#pragma pack(push, 1)

class Status
{
public:

	inline bool Valid()
	{
		return !_validFlag;
	}

	inline bool Error()
	{
		return _flags & 0x7E; 
	}

	inline bool Idle()
	{
		return _idle;
	}

	inline unsigned char Value()
	{
		return _flags;
	}

	inline bool EraseReset()
	{
		return _eraseReset;
	}

	inline bool IllegalCommand()
	{
		return _illegalCommand;
	}

	inline bool CrcError()
	{
		return _crcError;
	}

	inline bool EraseSequenceError()
	{
		return _eraseSequenceError;
	}

	inline bool AddressError()
	{
		return _addressError;
	}

	inline bool ParameterError()
	{
		return _parameterError;
	}

protected:

	union
	{
		unsigned char _flags;
		struct
		{
			unsigned char _idle : 1;
			unsigned char _eraseReset : 1;
			unsigned char _illegalCommand : 1;
			unsigned char _crcError : 1;
			unsigned char _eraseSequenceError : 1;
			unsigned char _addressError : 1;
			unsigned char _parameterError : 1;
			unsigned char _validFlag : 1;
		};
	};

};

//
// расширенный статус
//

class _StatusEx
{
protected:

	unsigned char _cardLocked : 1;
	unsigned char _wpEraseSkipOrLockUnlockFailed : 1;
	unsigned char _error : 1;
	unsigned char _controllerError : 1;
	unsigned char _cardEccFailed : 1;
	unsigned char _wpViolation : 1;
	unsigned char _eraseParam : 1;
	unsigned char _outOfRangeOrCsdOverwrite : 1;
};

class StatusEx : public _StatusEx, public Status
{
};

#pragma pack (pop)

}
}
}
}

#endif // _MEDIA_SD_RESPONSE_STATUS_H_

