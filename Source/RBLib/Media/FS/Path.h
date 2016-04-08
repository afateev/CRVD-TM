
#ifndef _MEDIA_FS_PATH_H_
#define _MEDIA_FS_PATH_H_

#include "NameSymbol.h"

namespace Rblib
{
namespace Media
{
namespace Fs
{

	namespace Path
	{
		// Путь указывает на корневой католог или нет
		static bool IsRoot(char* path)
		{
			if (path[0] == 0)
				return true;

			if (path[0] != '.')
				return false;

			unsigned long pos = 1;

			while(path[pos] == '/' || path[pos] == '\\') // пропускаем разделители
				pos++;

			while(path[pos] == '/' || path[pos] == '\\') // пропускаем пробелы
				pos++;

			return path[pos] == 0;
		}

	}

}
}
}

#endif // _MEDIA_FS_PATH_H_

