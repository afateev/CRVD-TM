///////////////////////////////////////////////////////////////////////////////
//	Базовый класс различных типов шрифтов
//	
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GDI_FONT_H
#define GDI_FONT_H

namespace Rblib
{
	namespace Gdi
	{
		template<class DeviceContext>
		class Font
		{
		public:
			// Нарисовать символ
			// dc - контекст рисования
			// chid - код символа
			// left - x - координата левого верхнего угла символа
			// top - y - координата левого верхнего угла символа
			// color  - цвет символа
			// Возвращает ширину нарисованного символа
			virtual int DrawChar(DeviceContext *dc, unsigned int chId, int left, int top, int color)
			{
				// базовый класс рисует прямоугольники 8х10 вместо символов
				dc->FrameRect(left, top, left + 8, top + 10);
				return 8;
			}

			// Получить размер символа
			// dc - контекст рисования
			// chid - код символа
			// width - возвращаемая ширина символа
			// heigth - возвращаемая высота символа
			virtual void GetCharSize(DeviceContext *dc, unsigned int chId, int &width, int &heigth)
			{
				width = 8;
				heigth = 10;
			}
		};
	}
}

#endif