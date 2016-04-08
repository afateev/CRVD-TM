
#ifndef _MEDIA_FS_FAT32_NAME_H_
#define _MEDIA_FS_FAT32_NAME_H_

#include "NameSymbol.h"

namespace Rblib
{
namespace Media
{
namespace Fs
{
namespace Fat32
{

	namespace Name
	{
		// варианты результатов проверки имени
		enum Type
		{
			Short = 1, // короткое
			Long = 2, // длинное
			NotValid // недопустимое
		};

		// проверка типа имени
		static Type Check(char* name, unsigned char len)
		{
			bool longName = false;
			bool lower = false;
			bool upper = true;
			bool point = false;
			unsigned char symbolsCount = 0; // короткого имени: имя макс. 8, расширение макс. 3

			for(int i = 0; i < len; i++)
			{
				// недопустимый для любового имени символ
				if (!Name::Symbol::Long(name[i]))
					return NotValid;

				// уже поняли, что имя длинное
				if (longName)
					continue;

				// точка и точка уже была
				if (name[i] == '.' && point)
				{
					longName = true;
					continue;
				}

				// первая точка - начинается расширение, начинаем сначала
				if (name[i] == '.')
				{
					symbolsCount = 0;
					lower = upper = false;
					point = true;
					continue;
				}

				symbolsCount++;

				// если уже была точка, то это расширение (<= 3 символов)
				if (point && symbolsCount > 3)
				{
					longName = true;
					continue;
				}

				// если не было точки, то это имя (<= 8 символов)
				if (!point && symbolsCount > 8)
				{
					longName = true;
					continue;
				}

				// недопустимый для короткого имени символ
				if (!Name::Symbol::Short(name[i]))
				{
					longName = true;
					continue;
				}

				if (Name::Symbol::Lower(name[i]))
				{
					if (upper) // буквы в раном регистре (в имени или расширении)
					{
						longName = true; // длинное имя
						continue;
					}
					lower = true;
				}
				else if (Name::Symbol::Upper(name[i]))
				{
					if (lower) // буквы в раном регистре (в имени или расширении)
					{
						longName = true; // длинное имя
						continue;
					}
					upper = true;
				}
			}

			return longName ? Long : Short;
		}

		// поиск последней точки
		unsigned char FindLastPoint(const char* name, unsigned char len)
		{
			for (short i = len - 1; i >= 0; i--)
			{
				if (name[i] == '.')
					return (unsigned char)i;
			}

			return len;
		}

		//
		unsigned char DigitsToString(unsigned long digits, char* buffer)
		{
			unsigned char currentDigit = 0;
			do 
			{
				buffer[currentDigit++] = (char) (digits % 10 + 48); // 48 - код символа '1' в ASCII
				digits /= 10;
			} while (digits > 0);

			return currentDigit;
		}

		// формирование короткого имени
		unsigned char ToShort(const char* name, unsigned char len, char* shortName)
		{
			unsigned char lastPoint = len;

			unsigned char currentChar = 0;

			// копируем имя
			for(; currentChar < len && currentChar < 8; currentChar++)
			{
				if (name[currentChar] == '.')
					lastPoint = currentChar;
				shortName[currentChar] = Name::Symbol::ToShort(name[currentChar]);
			}

			// если скопировано всё
			if (currentChar >= len && lastPoint != len && (len - lastPoint - 1) <= 3)
			{
				shortName[lastPoint] = '.'; // ставим последнюю точку (если не нужна, она будет затёрта нулём ниже)
				shortName[currentChar] = 0; // ставим конец строки
				return currentChar;
			}

			// ищем последнюю точку
			lastPoint = FindLastPoint(name, len);

			// точки нет => нет расширения, выходим
			if (lastPoint >= len)
			{
				shortName[currentChar] = 0;
				return currentChar;
			}

			currentChar = (lastPoint < currentChar) ? lastPoint : currentChar;
			shortName[currentChar++] = '.';
			lastPoint++;

			for(unsigned char i = 0; i < 3 && (i + lastPoint) < len; i++)
				shortName[currentChar++] = Name::Symbol::ToShort(name[i + lastPoint]);

			shortName[currentChar] = 0;

			return currentChar;
		}

		// добавление цифр в короткое имя
		// nameBuffer должен быть не менее 13 байт (вмещать имя в формате 8.3 + \0)
		unsigned char AddShortDigits(char* nameBuffer, unsigned char len, unsigned long digits)
		{
			const unsigned long maxDigits = 999999;
			
			if (digits > maxDigits)
				digits = maxDigits;

			char digitsBuffer[7];
			unsigned char digitsLen = DigitsToString(digits, digitsBuffer);
			digitsBuffer[digitsLen++] = '~';

			unsigned char lastPoint = FindLastPoint(nameBuffer, len);
			unsigned char nameLen = lastPoint;

			unsigned char shift = 8 - nameLen; // считаем, что имя корректное, т.е. nameLen <= 8
			shift = (shift < digitsLen) ? shift : digitsLen;

			if (lastPoint != len)
			{
				for(unsigned char i = 0; i < shift; i++)
					nameBuffer[len + shift - 1 - i] = nameBuffer[len - 1 - i];
				lastPoint += shift;
			}

			for(unsigned char i = 0; i < digitsLen; i++)
			{
				nameBuffer[lastPoint - 1 - i] = digitsBuffer[i];
			}

			nameBuffer[len + shift] = 0;

			return len + shift;
		}

	}

}
}
}
}

#endif // _MEDIA_FS_FAT32_NAME_H_

