///////////////////////////////////////////////////////////////////////////////
//	Классы управления разными частями контроллера флэш памяти
//
//	Разные наборы частей разделены по разным пространствам имён по платформам
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef FLASHMEMORYCONTROLLER_PARTS_MSP430_H
#define FLASHMEMORYCONTROLLER_PARTS_MSP430_H

#include "Common.h"

namespace Rblib
{
// Части специфичные для платформы MSP430 всех семейств
namespace Msp430
{
	// Тип значения регистра
	typedef unsigned short FlashRegisterDataType;
	// Тип адреса регистра
	typedef unsigned int FlashRegisterAddresType;
	// Тип указателя во флэш
	typedef unsigned long FlashPointerType;

	// Класс управления регистрами флэш контроллера
	template <class RegisterDataType,				// Тип (размер) значения регистра
			class PointerType,				// Тип (размер) значения адресов регистров
			class RegisterStructure,		// Структура регистра
			PointerType ControlRegisterAddr	// Адрес регистра управления
		>
	class FlashMemoryControllerRegister
	{
	protected:
		// Адрес регистра управления
		static volatile RegisterDataType * const _controlRegisterAddr;
	public:
		// Чтение значения регистра
		static inline void GetValue(RegisterStructure *value)
		{
			 memcpy(value, (const void*)_controlRegisterAddr, sizeof(RegisterDataType));
		}
		// Запись значения регистра
		static inline void SetValue(RegisterStructure *value)
		{
			// выставляем обязательный пароль на запись
			value->Password = 0xA5;
			// т.к. в регистр можно писать только целым словом
			// создаем это слово
			RegisterDataType tmp;
			// копируем туда значение структуры
			memcpy(&tmp, value, sizeof(RegisterDataType));
			// записываем в регистр
			*_controlRegisterAddr = tmp;
		}
	};

	// Инстанцирование статических переменных
	template<class RegisterDataType, class PointerType, class RegisterStructure, PointerType ControlRegisterAddr>
	volatile  RegisterDataType* const FlashMemoryControllerRegister<RegisterDataType, PointerType, RegisterStructure, ControlRegisterAddr>
		::_controlRegisterAddr = reinterpret_cast<volatile  RegisterDataType*>(ControlRegisterAddr);

	// Класс границ области флэш памяти
	template<class PointerType, PointerType start, PointerType size, PointerType segmentSize>
	class FlashMemorySegmentBounds
	{
	public:
		static const PointerType Start = start;
		static const PointerType Size = size;
		static const PointerType SegmentSize = segmentSize;
	};

	// Контроллер флэш памяти
	template<class PointerType,			// Тип указателей
			class RegisterDataType,		// Тип (размер) значения регистра
			class RegisterAddressType,	// Тип (размер) значения адресов регистров
			RegisterAddressType Control1Address,
			class Control1Structure,
			RegisterAddressType Control3Address,
			class Control3Structure,
			class InfoMemory,
			class MainMemory
		>
	class FlashMemoryController
	{
	protected:
		typedef FlashMemoryControllerRegister<RegisterDataType, RegisterAddressType, Control1Structure, Control1Address> Control1;
		typedef FlashMemoryControllerRegister<RegisterDataType, RegisterAddressType, Control3Structure, Control3Address> Control3;
	public:
		typedef InfoMemory Info;
		typedef MainMemory Main;
	public:
		// Прочитать байт по адресу
		static inline unsigned char Read(PointerType address)
		{
			return RBLIB_READ_FULL_ADDR_CHAR(address);
		}
		// Прочитать буфер по адресу
		static inline void Read(PointerType address, unsigned char *data, PointerType len)
		{
			if (0 == data)
				return;
			for (PointerType i = 0; i < len; i++)
			{
				data[i] = Read(address + i);
			}
		}
		// Получить стартовый адрес сегмента и его размер, по переданному адресу
		static inline void GetSegmentBounds(PointerType address, PointerType &segmentStart, PointerType &segmentSize)
		{
			PointerType start = 0;
			PointerType size = 0;
			// Info
			if (address >= InfoMemory::Start && address <= InfoMemory::Start + InfoMemory::Size)
			{
				start = InfoMemory::Start;		// начало области памяти
				size = InfoMemory::SegmentSize;	// размер сегментов
			}
			// Main memory
			if (address >= MainMemory::Start && address <= MainMemory::Start + MainMemory::Size)
			{
				start = MainMemory::Start;		// начало области памяти
				size = MainMemory::SegmentSize;	// размер сегментов
			}

			// если не попали в допустимую область
			if (size == 0)
			{
				// возвращаем нулевой размер
				segmentSize = size;
				return;
			}

			// вычисляем начало сегмента
			
			// вычисляем смещение от начала блока
			segmentStart = address - start;
			// сколько сегментов состаляет это смещение
			segmentStart /= size;
			// сколько целых сегментов
			segmentStart *= size;
			// добавляя начало к размеру целых сегментов получаем адрес начала сегмента
			segmentStart += start;
			// размер сегмента
			segmentSize = size;
		}
		// Стереть сегмент
		static inline void Erase(PointerType address)
		{       
			Control1Structure tmp1;
			Control3Structure tmp3;
			// проверяем на занятость
			do
			{
				Control3::GetValue(&tmp3);
			}
			while(tmp3.Busy);
			// разблокируем
			tmp3.Lock = 0;
			Control3::SetValue(&tmp3);
			// выставляем флаг стирания
			Control1::GetValue(&tmp1);
			tmp1.Erase = 1;
			Control1::SetValue(&tmp1);
			
			// делаем "типа запись", это вызовет затирание целого сегмента
			RBLIB_WRITE_FULL_ADDR_CHAR(address, 0);

			// ждём пока занято
			do
			{
				Control3::GetValue(&tmp3);
			}
			while(tmp3.Busy);
			// блокируем обратно
			tmp3.Lock = 1;
			Control3::SetValue(&tmp3);
		}
		// Записать байт по адресу
		static inline void WriteByte(PointerType address, unsigned char value)
		{
			Control1Structure tmp1;
			Control3Structure tmp3;
			// проверяем на занятость
			do
			{
				Control3::GetValue(&tmp3);
			}
			while(tmp3.Busy);
			// разблокируем
			Control3::GetValue(&tmp3);
			tmp3.Lock = 0;
			Control3::SetValue(&tmp3);
			// разрешаем запись
			Control1::GetValue(&tmp1);
			tmp1.Write = 1;
			Control1::SetValue(&tmp1);

			// записываем
			RBLIB_WRITE_FULL_ADDR_CHAR(address, value);
            
			// ждём пока занято
			do
			{
				Control3::GetValue(&tmp3);
			}
			while(tmp3.Busy);
			// выключаем запись
			Control1::GetValue(&tmp1);
			tmp1.Write = 0;
			Control1::SetValue(&tmp1);
			// блокируем обратно
			Control3::GetValue(&tmp3);
			tmp3.Lock = 1;
			Control3::SetValue(&tmp3);
		}
	};
}
}

#endif