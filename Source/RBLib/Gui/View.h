///////////////////////////////////////////////////////////////////////////////
//	Внешний вид элемента управления графического интерфейса пользователя
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GUI_VIEW_H
#define GUI_VIEW_H

#include "../Utils/map_static.h"
#include "ViewState.h"

namespace Rblib
{
namespace Gui
{
	class View
	{
	public:
		virtual void Draw(ViewState &viewState)
		{

		}
	};
}
}

#endif