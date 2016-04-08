///////////////////////////////////////////////////////////////////////////////
//	Класс управления сегментированной флэш памятью
//	Это такая память, которая для записи в сегмент
//	требует полного стирания сегмента
//	
//	Здесь функция записи делает следующее:
//	1. Определяет в какой сегмент будут записываться данные и его размер
//	2. Копирует текущее содержимое сегмента во временную память
//	3. Стирает сегмент целиком
//	4. Во время записи пользовательских данных одновременно восстанавливает
//		старые данные сегмента, которые не изменяются в результате
//		записи пользовательских данных
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef FLASHMEMORYSEGMENTED_H
#define FLASHMEMORYSEGMENTED_H

namespace Rblib
{
	template<class PointerType,				// тип указателей
			class FlashMemoryController,	// Контроллер флэш
			class TmpSpace					// Место для временной копии сегмента
		>
	class FlashMemorySegmented
	{
	public:
		// Записать данные по адресу
		// address - стартовый адрес
		// data - блок данных
		// len - длина блока данных
		static inline void Write(PointerType address, unsigned char *data, PointerType len)
		{
			// позиция в источнике данных
			PointerType srcPos = 0;
			// позиции начала и конца области записи флэшки
			PointerType writePos = address;
			PointerType writeEnd = address + len - 1;
			// начала и размер сегмента где находится начало блока
			PointerType firstSegmentStart;
			PointerType firstSegmentSize;
			// начала и размер сегмента где находится конец блока
			PointerType lastSegmentStart;
			PointerType lastSegmentSize;
			// начало текущего сегмента
			PointerType currentSegmentStart = 0;
			// в каком сегменте будем начинать запись
			FlashMemoryController::GetSegmentBounds(writePos, firstSegmentStart, firstSegmentSize);
			// в каком сегменте закончим запись
			FlashMemoryController::GetSegmentBounds(writeEnd, lastSegmentStart, lastSegmentSize);
			// если сюда нельзя записывать, выходим
			if (firstSegmentSize == 0 || lastSegmentSize == 0)
				return;
			do
			{
				// начало и размер сегмента куда сейчас пишем
				PointerType segmentStart;
				PointerType segmentSize;
				FlashMemoryController::GetSegmentBounds(writePos, segmentStart, segmentSize);
				// если начался новый сегмент
				if (currentSegmentStart != segmentStart)
				{
					currentSegmentStart = segmentStart;
					writePos = currentSegmentStart;
					// копируем сегмент
					TmpSpace::Write(0, (unsigned char *)segmentStart, segmentSize);
					// стираем сегмент
					FlashMemoryController::Erase(currentSegmentStart);
				}
				if (writePos < address)
				{
					FlashMemoryController::WriteByte(writePos, TmpSpace::ReadByte(writePos - currentSegmentStart));
				}
				else
				{
					FlashMemoryController::WriteByte(writePos, data[srcPos]);
					srcPos++;
				}

				writePos++;
			}
			while(writePos <= writeEnd);

			// дописываем оставшуюся часть последнего сегмента
			while(writePos < lastSegmentStart + lastSegmentSize)
			{
				FlashMemoryController::WriteByte(writePos, TmpSpace::ReadByte(writePos - lastSegmentStart));
				writePos++;
			}
		}
	};
}
#endif