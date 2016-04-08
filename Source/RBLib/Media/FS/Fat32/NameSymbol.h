
#ifndef _MEDIA_FS_FAT32_NAME_SYMBOL_H_
#define _MEDIA_FS_FAT32_NAME_SYMBOL_H_

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
	namespace Symbol
	{

		// ������� �������
		static inline bool Upper(char c)
		{
			return c >= 'A' && c <= 'Z';
		}

		// ������ �������
		static inline bool Lower(char c)
		{
			return c >= 'a' && c <= 'z';
		}

		// ������ �������� � �������� �����
		static inline bool Short(char c)
		{
			// ����� � �����
			if (c >= '0' && c <= '9' || c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z')
				return true;

			// �������
			if (c == '!' || c >= '#' && c <= ')' || c == '-' || c == '_')
				return true;
			
			// ���� ������ ��������
			//// ����������� ����
			//if (c > 127)
			//	return true;

			return false;
		}

		// ������ �������� � ������� �����
		static inline bool Long(char c)
		{
			if (Short(c))
				return true;

			if (c == ' ' || c == '+' || c == ',' || c == ';' || c == '=' || c == '[' || c == ']' || c == '.')
				return true;

			return false;
		}

		static inline char ToShort(char c)
		{
			if (Lower(c))
				return c - 32; // � ������� �������;
			if (!Short(c))
				return '_';
			return c;
		}

	}
	}

}
}
}
}

#endif // _MEDIA_FS_FAT32_NAME_SYMBOL_H_

