
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

		// верхний регистр
		static inline bool Upper(char c)
		{
			return c >= 'A' && c <= 'Z';
		}

		// нижний регистр
		static inline bool Lower(char c)
		{
			return c >= 'a' && c <= 'z';
		}

		// символ разрешён в коротком имени
		static inline bool Short(char c)
		{
			// цифры и буквы
			if (c >= '0' && c <= '9' || c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z')
				return true;

			// символы
			if (c == '!' || c >= '#' && c <= ')' || c == '-' || c == '_')
				return true;
			
			// пока только латиница
			//// расширенные коды
			//if (c > 127)
			//	return true;

			return false;
		}

		// символ разрешён в длинном имени
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
				return c - 32; // в верхний регистр;
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

