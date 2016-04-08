
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
		// �������� ����������� �������� �����
		enum Type
		{
			Short = 1, // ��������
			Long = 2, // �������
			NotValid // ������������
		};

		// �������� ���� �����
		static Type Check(char* name, unsigned char len)
		{
			bool longName = false;
			bool lower = false;
			bool upper = true;
			bool point = false;
			unsigned char symbolsCount = 0; // ��������� �����: ��� ����. 8, ���������� ����. 3

			for(int i = 0; i < len; i++)
			{
				// ������������ ��� �������� ����� ������
				if (!Name::Symbol::Long(name[i]))
					return NotValid;

				// ��� ������, ��� ��� �������
				if (longName)
					continue;

				// ����� � ����� ��� ����
				if (name[i] == '.' && point)
				{
					longName = true;
					continue;
				}

				// ������ ����� - ���������� ����������, �������� �������
				if (name[i] == '.')
				{
					symbolsCount = 0;
					lower = upper = false;
					point = true;
					continue;
				}

				symbolsCount++;

				// ���� ��� ���� �����, �� ��� ���������� (<= 3 ��������)
				if (point && symbolsCount > 3)
				{
					longName = true;
					continue;
				}

				// ���� �� ���� �����, �� ��� ��� (<= 8 ��������)
				if (!point && symbolsCount > 8)
				{
					longName = true;
					continue;
				}

				// ������������ ��� ��������� ����� ������
				if (!Name::Symbol::Short(name[i]))
				{
					longName = true;
					continue;
				}

				if (Name::Symbol::Lower(name[i]))
				{
					if (upper) // ����� � ����� �������� (� ����� ��� ����������)
					{
						longName = true; // ������� ���
						continue;
					}
					lower = true;
				}
				else if (Name::Symbol::Upper(name[i]))
				{
					if (lower) // ����� � ����� �������� (� ����� ��� ����������)
					{
						longName = true; // ������� ���
						continue;
					}
					upper = true;
				}
			}

			return longName ? Long : Short;
		}

		// ����� ��������� �����
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
				buffer[currentDigit++] = (char) (digits % 10 + 48); // 48 - ��� ������� '1' � ASCII
				digits /= 10;
			} while (digits > 0);

			return currentDigit;
		}

		// ������������ ��������� �����
		unsigned char ToShort(const char* name, unsigned char len, char* shortName)
		{
			unsigned char lastPoint = len;

			unsigned char currentChar = 0;

			// �������� ���
			for(; currentChar < len && currentChar < 8; currentChar++)
			{
				if (name[currentChar] == '.')
					lastPoint = currentChar;
				shortName[currentChar] = Name::Symbol::ToShort(name[currentChar]);
			}

			// ���� ����������� ��
			if (currentChar >= len && lastPoint != len && (len - lastPoint - 1) <= 3)
			{
				shortName[lastPoint] = '.'; // ������ ��������� ����� (���� �� �����, ��� ����� ������ ���� ����)
				shortName[currentChar] = 0; // ������ ����� ������
				return currentChar;
			}

			// ���� ��������� �����
			lastPoint = FindLastPoint(name, len);

			// ����� ��� => ��� ����������, �������
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

		// ���������� ���� � �������� ���
		// nameBuffer ������ ���� �� ����� 13 ���� (������� ��� � ������� 8.3 + \0)
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

			unsigned char shift = 8 - nameLen; // �������, ��� ��� ����������, �.�. nameLen <= 8
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

