
#ifndef _BIT_FLAGS_HOLDER_H_
#define _BIT_FLAGS_HOLDER_H_

template<unsigned short MaxFlagsNumber>
class _BitFlagsHolder
{
public:

	//
	// инициализация
	//

	_BitFlagsHolder() : _size(0)
	{
		Clear();
	}

	_BitFlagsHolder(unsigned short flagsNumber) : _size(0)
	{
		SetFlagsNumber(flagsNumber);
		Clear();
	}

	_BitFlagsHolder(const _BitFlagsHolder& flagsHolder)
	{
		Copy(flagsHolder);
	}

	_BitFlagsHolder& operator = (const _BitFlagsHolder& flagsHolder)
	{
		Copy(flagsHolder);

		return *this;
	}

	// установить используемое количество флагов
	inline bool SetFlagsNumber(unsigned short flagsNumber)
	{
		if (flagsNumber > MaxFlagsNumber)
			return false;

		_size = BufferSize(flagsNumber);

		return true;
	}

	//
	// считывание значений флагов
	//

	inline bool Get(unsigned short f)
	{
		return _flagsBuffer[FlagByte(f)] & FlagBit(f);
	}

	//
	// установка значений флагов
	//

	// установить конкретный флаг
	inline void Set(unsigned char f)
	{
		_flagsBuffer[FlagByte(f)] |= FlagBit(f);
	}

	// сбросить конкретный флаг
	inline void Clear(unsigned char f)
	{
		_flagsBuffer[FlagByte(f)] &= ~FlagBit(f);
	}

	// установить все флаги
	inline void Set()
	{
		for (int i = 0; i < _bufferSize; i++)
			_flagsBuffer[i] = (unsigned char) -1;
	}

	// сбросить все флаги
	inline void Clear()
	{
		for (int i = 0; i < _bufferSize; i++)
			_flagsBuffer[i] = 0;
	}

	// во внешнее представление
	unsigned char Serialize(unsigned char* buffer)
	{
		buffer[0] = _size;
		for (int i = 0; i < _size; i++)
		{
			buffer[i + 1] = _flagsBuffer[i];
		}

		return _size + 1;
	}

	// размер буфера, в который влезет объект
	inline unsigned char SerializedSize()
	{
		return _size + 1;
	}

	// из внешнего представления
	void Deserialize(unsigned char* buffer, unsigned char size)
	{
		if (size < 1 || buffer[0] != (size - 1) || _bufferSize < (size - 1) )
			return;

		_size = buffer[0]; 

		for (int i = 1; i < size; i++)
		{
			_flagsBuffer[i - 1] = buffer[i];
		}
	}

	inline unsigned char Size()
	{
		return _size;
	}

protected:

	static inline unsigned char FlagByte(unsigned short f)
	{
		return f / 8;
	}

	static inline unsigned char FlagBit(unsigned char f)
	{
		return (1 << (f % 8)); // маска для выделения нужного бита
	}

	static inline unsigned char BufferSize(unsigned short flagsNumber)
	{
		return flagsNumber / 8 + ((flagsNumber % 8) ? 1 : 0);
	}

	// копирует только используемые данные
	inline void Copy(const _BitFlagsHolder& flagsHolder)
	{
		_size = (flagsHolder._size > _bufferSize) ? _bufferSize : flagsHolder._size;

		for (int i = 0; i < _size; i++)
			_flagsBuffer[i] = flagsHolder._flagsBuffer[i];
	}

protected:

	static const unsigned char _bufferSize = MaxFlagsNumber / 8 + ((MaxFlagsNumber % 8) ? 1 : 0); // зарезервированный (максимальный) размер буфера

	unsigned char _size; // используемый размер буфера
	unsigned char _flagsBuffer[_bufferSize];

};

typedef _BitFlagsHolder<704> BitFlagsHolder; // занимает 88 байт

#endif // _BIT_FLAGS_HOLDER_H_

