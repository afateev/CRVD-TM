
#ifndef _MEDIA_FS_PATH_ITERATOR_H_
#define _MEDIA_FS_PATH_ITERATOR_H_

namespace Rblib
{
namespace Media
{
namespace Fs
{

class PathIterator
{
public:

	PathIterator(const char* str)
	: _str(str)
	, _currentPosition(-1)
	{
		Begin();
	}

	void Begin()
	{
		SetToBeginning();

		// достигнут конец строки?
		if (CheckStrEnd())
			return;
		
		_lastPoint = _nameBufferLastIndex;

		ProcessName();
	}

	void Next()
	{
		SkipSeparator();
		SkipSpaces();

		// достигнут конец строки
		if (CheckStrEnd())
			return;

		ProcessName();
	}

	inline bool End()
	{
		if (_currentPosition < 0)
			return true;
		return false;
	}

	inline char* Name()
	{
		return _currentName;
	}

	inline unsigned long NameLength()
	{
		return _currentNameLength;
	}

	inline bool File()
	{
		return _file;
	}

	inline unsigned long ExtensionSeparatorPosition()
	{
		return _lastPoint;
	}

protected:

	void SetToBeginning()
	{
		_currentPosition = 0;

		// удалить начальные пробелы всей строки
		SkipSpaces();

		// удалить начальные разделители
		SkipSeparator();

		// удалить начальные пробелы в имени
		SkipSpaces();
	}

	// пропустить последовательность разделителей
	inline void SkipSeparator()
	{
		while(_str[_currentPosition] == '/' || _str[_currentPosition] == '\\')
			_currentPosition++;
	}

	// пропустить последовательность пробелов
	inline void SkipSpaces()
	{
		while(_str[_currentPosition] == ' ')
			_currentPosition++;
	}

	bool CheckStrEnd()
	{
		// достигнут конец строки
		if (_str[_currentPosition] == 0)
		{
			_currentPosition = -1;
			return true;
		}

		return false;
	}

	void ProcessName()
	{
		int rightSpasesStart = _nameBufferLastIndex;
		_lastPoint = _nameBufferLastIndex;

		int i = 0;

		for(;_str[_currentPosition] != 0 
			&& _str[_currentPosition] != '\\'  
			&& _str[_currentPosition] != '/' 
			&& i < _nameBufferLastIndex; i++, _currentPosition++)
		{
			_currentName[i] = _str[_currentPosition];

			// чтобы отфильтровать пробелы в конце
			if (_str[_currentPosition] == ' ')
			{
				if (rightSpasesStart == _nameBufferLastIndex)
					rightSpasesStart = i; // если 1-й пробел в серии запоминаем позицию
			}
			else
			{
				rightSpasesStart = _nameBufferLastIndex; // если не пробел, "забываем" позицию пробела
			}

			if (_str[_currentPosition] == '.')
				_lastPoint = i;
		}

		if (i < rightSpasesStart)
			_currentNameLength = i;
		else
			_currentNameLength = rightSpasesStart;

		_currentName[_currentNameLength] = 0;

		if (_str[i] == '\\' || _str[i] == '/')
		{
			_file = false;
		}
		else
			_file = true;
	}

protected:
	static const unsigned long _nameBufferLength = 256;
	static const unsigned long _nameBufferLastIndex = _nameBufferLength - 1;

	unsigned long _currentNameLength;
	char _currentName[_nameBufferLength];
	bool _file;
	unsigned long _lastPoint;

	const char* _str;
	int _currentPosition;
};

}
}
}

#endif // _MEDIA_FS_PATH_ITERATOR_H_

