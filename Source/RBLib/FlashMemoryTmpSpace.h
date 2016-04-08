///////////////////////////////////////////////////////////////////////////////
//	Класс управления областью флэш памяти для временного хранения данных
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef FLASHMEMORYTMPSPACE_H
#define FLASHMEMORYTMPSPACE_H

namespace Rblib
{
	template<class PointerType,					// Тип (размер) значения адресов
			PointerType StartAddress,			// Начало области
			PointerType Length,					// Размер области
			class FlashMemoryController			// Контроллер флэш
		>
	class FlashMemoryTmpSpace
	{
	public:
		// Запись данных во временную область
		static inline void Write(PointerType address, unsigned char *data, PointerType len)
		{
			// позиция в источнике данных
			PointerType srcPos = 0;
			// позиции начала и конца области записи флэшки
			PointerType writePos = StartAddress + address;
			PointerType writeEnd = writePos + len - 1;
			// начало текущего сегмента
			PointerType currentSegmentStart = 0;
			// начало и размер сегмента куда сейчас пишем
			PointerType segmentStart = 0;
			PointerType segmentSize = 0;
			do
			{
				FlashMemoryController::GetSegmentBounds(writePos, segmentStart, segmentSize);
				// если начался новый сегмент
				if (currentSegmentStart != segmentStart)
				{
					currentSegmentStart = segmentStart;
					writePos = currentSegmentStart;
					// стираем сегмент
					FlashMemoryController::Erase(currentSegmentStart);
				}
				FlashMemoryController::WriteByte(writePos, data[srcPos]);
				srcPos++;
				writePos++;
			}
			while(writePos <= writeEnd);
		}
		// Прочитать данные из временной области
		static inline unsigned char ReadByte(PointerType address)
		{
			return FlashMemoryController::Read(StartAddress + address);
		}
	};
}

#endif