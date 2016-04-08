
#ifndef _MEDIA_FS_FAT32_LONG_NAME_PART_H_
#define _MEDIA_FS_FAT32_LONG_NAME_PART_H_

namespace Rblib
{
namespace Media
{
namespace Fs
{
namespace Fat32
{

class LongNamePart
{

public:

	LongNamePart(char* str, unsigned char len)
	: _str(str)
	, _len(len)
	, _currentPartBegin(len)
	, _currentPartLen(0)
	{
		Begin();
	}

	inline void Begin()
	{
		_currentPartLen = _len % _partLen;

		if (_currentPartLen == 0)
			_currentPartLen = _partLen;

		_currentPartBegin = _len - _currentPartLen;

		ConvertAndCopy();
	}

	inline void Next()
	{
		_currentPartBegin -= _partLen;
		_currentPartLen = _partLen;

		if (_currentPartBegin < 0)
			return;

		ConvertAndCopy();
	}

	inline bool End()
	{
		return _currentPartBegin < 0;
	}

	inline char* Get()
	{
		return _str + _currentPartBegin;
	}

	inline unsigned short* GetUnicode()
	{
		return _partBuffer;
	}

	inline unsigned char Length()
	{
		return _currentPartLen;
	}

	static inline unsigned char Count(unsigned char nameLen)
	{
		return nameLen / _partLen + ((nameLen % _partLen) ? 1 : 0);
	}

protected:

	inline void ConvertAndCopy()
	{
		for (unsigned char i = 0; i < _currentPartLen; i++)
			_partBuffer[i] = _str[_currentPartBegin + i];
	}

private:

	char* _str;
	unsigned char _len;

	short _currentPartBegin;
	unsigned char _currentPartLen;

	static const unsigned char _partLen = 13;
	unsigned short _partBuffer[_partLen]; // буфер под кусок имени в юникоде

};

}
}
}
}

#endif // _MEDIA_FS_FAT32_LONG_NAME_PART_H_

