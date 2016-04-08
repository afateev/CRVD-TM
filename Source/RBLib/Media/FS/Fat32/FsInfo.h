
#ifndef _MEDIA_FS_FAT32_FS_INFO_H_
#define _MEDIA_FS_FAT32_FS_INFO_H_

namespace Rblib
{
namespace Media
{
namespace Fs
{
namespace Fat32
{

#pragma pack(push, 1)

class FsInfo
{
public:

	inline bool Valid()
	{
		if (_leadSignature == _leadSigValue && _signature == _sigValue && _trailSignature == _trailSigValue)
			return true;
		return false;
	}

	inline void Reset()
	{
		memset(this, 0, sizeof(FsInfo));
		SetValid();
		SetUnknownValues();
	}

	inline bool FreeCountValid()
	{
		return _freeCount != _unknownValue;
	}

	inline bool NextFreeValid()
	{
		return _nextFree != _unknownValue;
	}

	inline unsigned long NextFree()
	{
		return _nextFree;
	}

	inline unsigned long FreeCount()
	{
		return _freeCount;
	}

	inline void SetFreeCount(unsigned long freeCount)
	{
		_freeCount = freeCount;
	}

	inline void SetNextFree(unsigned long nextFree)
	{
		_nextFree = nextFree;
	}

	inline void SetUnknownValues()
	{
		_nextFree = _unknownValue;
		_freeCount = _unknownValue;
	}

	inline void SetUnknownValues(bool nextFree, bool freeCount)
	{
		if (nextFree)
			_nextFree = _unknownValue;
		if (freeCount)
			_freeCount = _unknownValue;
	}

	inline void SetValid()
	{
		_leadSignature = _leadSigValue;
		_signature = _sigValue;
		_trailSignature = _trailSigValue;
	}

private:

	static const unsigned long _leadSigValue = 0x41615252;
	static const unsigned long _sigValue = 0x61417272;
	static const unsigned long _trailSigValue = 0xAA550000;
	static const unsigned long _unknownValue = (unsigned long) -1;


	unsigned long _leadSignature;
	unsigned char _reserved1[480];
	unsigned long _signature;
	unsigned long _freeCount;
	unsigned long _nextFree;
	unsigned char _reserved2[12];
	unsigned long _trailSignature;

};

#pragma pack(pop)

}
}
}
}

#endif // _MEDIA_FS_FAT32_FS_INFO_H_

