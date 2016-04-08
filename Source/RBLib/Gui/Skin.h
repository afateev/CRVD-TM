///////////////////////////////////////////////////////////////////////////////
//	Набор внешних видов элементов управления графического интерфейса пользователя
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GUI_SKIN_H
#define GUI_SKIN_H

#include "View.h"

namespace Rblib
{
namespace Gui
{
	class Skin
	{
	public:
		virtual View * GetView(int id)
		{
			return 0;
		}

		virtual char *GetString(int id, int *strLen)
		{
			if (strLen)
			{
				*strLen = 0;
			}
			return 0;
		}

		virtual void *GetImageDump(int id)
		{
			return 0;
		}

		virtual int GetColor(int colorIndex)
		{
			return 0;
		}
	};
}
}

#endif