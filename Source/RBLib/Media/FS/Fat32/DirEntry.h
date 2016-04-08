
#ifndef _MEDIA_FS_FAT32_DIR_ENTRY_H_
#define _MEDIA_FS_FAT32_DIR_ENTRY_H_

#include "NameSymbol.h"

namespace Rblib
{
namespace Media
{
namespace Fs
{
namespace Fat32
{

class DirEntry
{
public:

	enum Attr
	{
		AttrReadOnly = 1,
		AttrHidden = 2,
		AttrSystem = 4,
		AttrVolumeId = 8,
		AttrDirectory = 16,
		AttrArchive = 32,
		AttrLongName = AttrReadOnly | AttrHidden | AttrSystem | AttrVolumeId
	};

public:

	// создаём как пустую запись, Valid() вернёт false
	DirEntry()
	: _order(_invalidOrder)
	{
		_clusterLow = 0;
		_clusterHigh = 0;
	}

	//
	// получение значений
	//
	
	// содержит ли запись информацию о файле или каталоге?
	inline bool Valid() const
	{
		return _order != _invalidOrder;
	}

	inline bool Empty() const
	{
		return _order == 0xE5 || _order == 0;
	}

	inline bool Free() const
	{
		return _order == 0xE5;
	}

	// запись свободна и после неё нет занятых
	inline bool LastInDir() const
	{
		return _order == 0;
	}

	inline bool AllEmpty() const
	{
		return _order == 0;
	}

	inline bool Directory() const
	{
		return (_attributes & AttrDirectory) ? true : false;
	}

	inline bool File() const
	{
		return (_attributes & AttrDirectory) ? false : true;
	}

	inline bool Long() const
	{
		return (_attributes & AttrLongName) ? true : false;
	}

	inline bool Short() const
	{
		return !Long();
	}

	inline bool Last() const
	{
		return (_order & _lastMask) ? true : false;
	}

	inline bool First() const
	{
		return (_order & _orderMask) == 1;
	}

	inline unsigned char Order() const
	{
		return _order & _orderMask;
	}

	inline unsigned long Cluster() const
	{
		return (unsigned long) _clusterLow | (unsigned long) _clusterHigh << 16;
	}

	inline unsigned long Size() const
	{
		return _fileSize;
	}

	// контрольная сумма для длинной записи
	inline unsigned char CheckSumLong() const
	{
		return _checkSum;
	}

	// контрольная сумма для короткой записи
	unsigned char CheckSumShort() const
	{
		unsigned char sum = 0;

		for (unsigned char i = 0; i < 11; i++)
		{
			sum = ((sum & 1) ? 0x80 : 0) + (sum >> 1) + _name[i];
		}

		return sum;
	}

	// контрольная сумма, универсальный метод
	inline unsigned char CheckSum() const
	{
		if (Long())
			return _checkSum;
		else
			return CheckSumShort();
	}

	// длинное имя
	// записывает в буфер склеенное имя, возвращает количество записанных символов
	// 0 в конце строки не ставит, т.к. в общем случае придётся склеивать имя из имён нескольких записей
	unsigned char Name(unsigned short* nameBuf) const
	{
		if (!Long())
			return 0;

		unsigned char currentBufIndex = 0;
		const unsigned short* namePart[] = {_name1, _name2, _name3};
		unsigned char namePartLen[] = {5, 6, 2};

		for(char p = 0; p < 3; p++)
		{
			for(char i = 0; i < namePartLen[p]; i++, currentBufIndex++)
			{
				if (namePart[p][i] == 0)
					return currentBufIndex;
				nameBuf[currentBufIndex] = namePart[p][i];
			}
		}

		return currentBufIndex;
	}

	// возвращает длинное имя преобразованное 
	unsigned char NameReverse(char* nameBuf, unsigned char nameBufLen)
	{	
		const unsigned short* namePart[] = {_name3, _name2, _name1};
		unsigned char namePartLen[] = {2, 6, 5};

		unsigned char outIndex = nameBufLen;

		for(char p = 0; p < 3; p++)
		{
			for(char i = namePartLen[p]; i > 0; i--)
			{
				if (!outIndex)
					return nameBufLen;

				if (namePart[p][i - 1] == 0 || namePart[p][i - 1] == 0xffff)
					continue;
				
				nameBuf[outIndex-- - 1] = (char) namePart[p][i - 1];
			}
		}

		return nameBufLen - outIndex;
	}

	// короткое имя с 0-м в конце
	// возвращает длину строки
	unsigned char Name(char* nameBuf) const
	{
		if (Long())
			return 0;

		unsigned char bufIndex = 0;

		bool nameInLowerCase = (_ntRes & _NameInLower) ? true : false;

		// копируем имя
		for(unsigned char i = 0; i < 8; i++, bufIndex++)
		{
			if (_name[i] == ' ')
				break;
			nameBuf[bufIndex] = _name[i] + ((nameInLowerCase && Name::Symbol::Upper(_name[i])) ? 32 : 0);
		}

		if (_name[10] == ' ') // если нет расширения
		{
			nameBuf[bufIndex] = 0;
			return bufIndex;
		}

		nameBuf[bufIndex++] = '.';

		// расширение выравнено по правому краю, ищем начало
		unsigned char extStartIndex = (_name[9] == ' ') ? 10 : ((_name[8] == ' ') ? 9 : 8);
		bool extInLowerCase = (_ntRes & _ExtInLower) ? true : false;

		// копируем расширение
		for(unsigned char i = extStartIndex; i < 11; i++, bufIndex++)
		{
			nameBuf[bufIndex] = _name[i] + ((extInLowerCase && Name::Symbol::Upper(_name[i])) ? 32 : 0);
		}

		nameBuf[bufIndex] = 0;

		return bufIndex;
	}

	//
	// установка значений
	//

	// переписывает нулями всю структуру
	void Clear()
	{
		// структура занимает 32 байта или 8 4-байтных целых
		for(unsigned char i = 0; i < 8; i++)
			((unsigned long*)_name)[i] = 0;
	}

	inline void SetAttributes(unsigned char attr)
	{
		_attributes = attr;
	}

	inline void SetDirectoryAttr()
	{
		_attributes |= AttrDirectory;
	}

	// установить атрибут "длинное имя"
	inline void SetLongNameAttr()
	{
		_attributes = AttrLongName;
	}

	void SetCheckSum(unsigned char checkSum)
	{
		_checkSum = checkSum;
	}

	void SetOrder(unsigned char order)
	{
		_order = _orderMask & order;
	}

	void SetLast()
	{
		_order |= _lastMask;
	}

	inline void SetSize(unsigned long sz)
	{
		_fileSize = sz;
	}

	// задать длинное имя
	void SetLongName(unsigned short* name, unsigned char len)
	{
		unsigned char inputIndex = 0;
		
		unsigned short* namePart[] = {_name1, _name2, _name3};
		unsigned char namePartLen[] = {5, 6, 2};

		for (unsigned char p = 0; p < 3; p++)
		{
			for (unsigned char i = 0; i < namePartLen[p]; i++, inputIndex++)
			{
				if (inputIndex < len)
					namePart[p][i] = name[inputIndex];
				else if (inputIndex == len)
					namePart[p][i] = 0;
				else
					namePart[p][i] = 0xFFFF;
			}
		}
	}

	//входная строка должна являться корректным коротким именем,
	//если для данной строки требуется хранить динное имя, будет возвращена ошибка
	bool SetShortName(char* name, unsigned char len)
	{
		if (name[0] == 0) // пустое имя
			return false;

		_ntRes = 0;

		signed char pointPos = CopyShortName(name, len);
		if (pointPos < 0)
			return false;

		signed char lastEmptyPos = _ShortNameLen - 1;
		if (pointPos < len)
			lastEmptyPos = CopyShortExt(name, len, pointPos);

		if (lastEmptyPos < 0)
			return false;

		for(; pointPos <= lastEmptyPos; pointPos++)
			_name[pointPos] = ' ';

		return true;
	}

	void SetCluster(unsigned long cluster)
	{
		_clusterLow = (unsigned short) cluster;
		_clusterHigh = (unsigned short) (cluster >> 16);
	}

	void SetAsCurrentDirEntry(unsigned long cluster)
	{
		_name[0] = '.';
		for(unsigned char i = 1; i < _ShortNameLen; i++)
			_name[i] = ' ';

		SetDirectoryAttr();
		SetCluster(cluster);
		_fileSize = 0;
	}

	void SetAsParentDirEntry(unsigned long cluster)
	{
		SetAsCurrentDirEntry(cluster);
		_name[1] = '.';
	}

	inline void SetEmpty()
	{
		_order = 0xE5;
	}

	// дата и время создания

	inline void SetCreateDate(unsigned short year, unsigned char month, unsigned char day)
	{
		_date.Set(year, month, day);
	}

	inline void SetCreateTime(unsigned char hours, unsigned char minutes, unsigned char seconds)
	{
		_time.Set(hours, minutes, seconds);
	}

	inline void SetCreateDateWise(unsigned short year, unsigned char month, unsigned char day)
	{
		SetCreateDate(year, month, day);
		SetWriteDateWise(year, month, day);
	}

	inline void SetCreateTimeWise(unsigned char hours, unsigned char minutes, unsigned char seconds)
	{
		SetCreateTime(hours, minutes, seconds);
		SetWriteTime(hours, minutes, seconds);
	}

	// дата и время последней записи

	inline void SetWriteDate(unsigned short year, unsigned char month, unsigned char day)
	{
		_writeDate.Set(year, month, day);
	}

	inline void SetWriteTime(unsigned char hours, unsigned char minutes, unsigned char seconds)
	{
		_writeTime.Set(hours, minutes, seconds);
	}

	inline void SetWriteDateWise(unsigned short year, unsigned char month, unsigned char day)
	{
		SetWriteDate(year, month, day);
		SetLastAccessDate(year, month, day);
	}

	// дата последнего обращения

	inline void SetLastAccessDate(unsigned short year, unsigned char month, unsigned char day)
	{
		_lastAccessDate.Set(year, month, day);
	}

protected:

	// это запись о каталоге ".."?
	inline bool DoublePoint() const
	{
		// если имя = ".." и нет расширения
		return Short() && _name[0] == '.' && _name[1] == '.' && _name[2] == 0x20 && _name[8] == 0x20;
	}

	//
	// вспомогательные функции для корректной установки короткого имени
	//

	signed char CopyShortName(char* name, unsigned char len)
	{
		bool inUpperCase = false;
		bool inLowerCase = false;
		char namePos = 0;

		// копируем имя
		for (; namePos < 8 && namePos < len && name[namePos] != '.'; namePos++)
		{
			if (Name::Symbol::Upper(name[namePos]))
			{
				if (inLowerCase)
					return -1;
				inUpperCase = true;
				_name[namePos] = name[namePos];
			}
			else if (Name::Symbol::Lower(name[namePos]))
			{
				if (inUpperCase)
					return -1;
				inLowerCase = true;
				_name[namePos] = name[namePos] - 32;
			}
			else
			{
				_name[namePos] = name[namePos];
			}
		}

		if (namePos < len && name[namePos] != '.')
			return -1;

		// ставим флаг, если имя было в нижнем регистре
		if (inLowerCase)
			_ntRes |= _NameInLower;

		return namePos;
	}

	signed char CopyShortExt(char* name, unsigned char len, unsigned char pointPos)
	{
		bool inUpperCase = false;
		bool inLowerCase = false;
		
		unsigned char extLen = len - pointPos - 1;
		if (extLen > 3)
			return false;

		char outIndex = _ShortNameLen - 1;

		for (char extPos = len - 1; extPos > pointPos; extPos--, outIndex--)
		{
			if (Name::Symbol::Upper(name[extPos]))
			{
				if (inLowerCase)
					return -1;
				inUpperCase = true;
				_name[outIndex] = name[extPos];
			}
			else if (Name::Symbol::Lower(name[extPos]))
			{
				if (inUpperCase)
					return -1;
				inLowerCase = true;
				_name[outIndex] = name[extPos] - 32;
			}
			else
			{
				_name[outIndex] = name[extPos];
			}
		}

		if (inLowerCase)
			_ntRes |= _ExtInLower;

		return outIndex;
	}

private:

#pragma pack(push, 1)
  
	struct Date
	{
		unsigned short _day : 5;
		unsigned short _month : 4;
		unsigned short _year : 7;

		inline void Set(unsigned short year, unsigned char month, unsigned char day)
		{
			_day = day;
			_month = month;
			_year = year - 1980;
		}
	};

	struct Time
	{
		unsigned short _2seconds : 5;
		unsigned short _minutes : 6;
		unsigned short _hours : 5;

		inline void Set(unsigned char hours, unsigned char minutes, unsigned char seconds)
		{
			_2seconds = seconds  >> 1;
			_minutes = minutes;
			_hours = hours;
		}
	};

	union
	{
		struct // структура для короткого имени
		{
			char _name[11];
			unsigned char _attributes;
			unsigned char _ntRes;
			unsigned char _timeTenth;
			Time _time;
			Date _date;
			Date _lastAccessDate;
			unsigned short _clusterHigh;
			Time _writeTime;
			Date _writeDate;
			unsigned short _clusterLow;
			unsigned long _fileSize;
		};

		struct // структура для длинного имени
		{
			unsigned char _order;
			unsigned short _name1[5];
			unsigned char : 8; // атрибуты (см. короткую структуру)
			unsigned char _type;
			unsigned char _checkSum;
			unsigned short _name2[6];
			unsigned short : 16; // кластер, младшая часть (см. короткую структуру)
			unsigned short _name3[2];
		};
	};

#pragma pack(pop)	
	
	static const unsigned char _NameInLower = 0x08;
	static const unsigned char _ExtInLower = 0x10;
	static const unsigned char _ShortNameLen = 11;
	static const unsigned char _orderMask = 0x3F;
	static const unsigned char _lastMask = 0x40;
	// значение 0-го байта структуры, некорректное и для короткой, и для длинной записи
	static const unsigned char _invalidOrder = 0x7C;
};

}
}
}
}

#endif // _MEDIA_FS_FAT32_DIR_ENTRY_H_

