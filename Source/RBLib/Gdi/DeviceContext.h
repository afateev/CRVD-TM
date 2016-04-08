///////////////////////////////////////////////////////////////////////////////
//	Контекст графического устройства
//	реализует методы рисования
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GDI_DEVICECONTEXT_H
#define GDI_DEVICECONTEXT_H

#include "Font.h"
#include "FontGlcd/FontGlcd.h"
#include "BmpReader.h"
#include "JpegReader.h"

namespace Rblib
{
namespace Gdi
{
	// Реализация SetPixelImpl
	// В зависимости от driverImplemented, выполняется драйвером или эмулируется контекстом устройтсва 
	template<class DeviceContext, class DeviceDriver, bool driverImplemented>
	class SetPixelImpl
	{
	};
	
	// Реализация MoveToImpl
	// В зависимости от driverImplemented, выполняется драйвером или эмулируется контекстом устройтсва
	template<class DeviceContext, class DeviceDriver, bool driverImplemented>
	class MoveToImpl
	{
	};
	
	// Реализация LineToImpl
	// В зависимости от driverImplemented, выполняется драйвером или эмулируется контекстом устройтсва
	template<class DeviceContext, class DeviceDriver, bool driverImplemented>
	class LineToImpl
	{
	};

	// Реализация FillRectImpl
	// В зависимости от driverImplemented, выполняется драйвером или эмулируется контекстом устройтсва
	template<class DeviceContext, class DeviceDriver, bool driverImplemented>
	class FillRectImpl
	{
	};

	// Реализация FrameRectImpl
	// В зависимости от driverImplemented, выполняется драйвером или эмулируется контекстом устройтсва
	template<class DeviceContext, class DeviceDriver, bool driverImplemented>
	class FrameRectImpl
	{
	};

	// Реализация RectangleImpl
	// В зависимости от driverImplemented, выполняется драйвером или эмулируется контекстом устройтсва
	template<class DeviceContext, class DeviceDriver, bool driverImplemented>
	class RectangleImpl
	{
	};
		
	// Контекст усторйтсва
	template<class DeviceDriver>
	class DeviceContext
	{
	public:
		typedef DeviceContext<DeviceDriver> MyType;
		typedef Font<MyType> FontBaseType;
		typedef FontGlcd<MyType> FontGlcdType;
		// Выбор реализации в зависимости от возможностей драйвера
		typedef SetPixelImpl<MyType, DeviceDriver, DeviceDriver::DeviceCaps::SetPixel> SetPixelImpl;
		typedef MoveToImpl<MyType, DeviceDriver, DeviceDriver::DeviceCaps::MoveTo> MoveToImpl;
		typedef LineToImpl<MyType, DeviceDriver, DeviceDriver::DeviceCaps::LineTo> LineToImpl;
		typedef FillRectImpl<MyType, DeviceDriver, DeviceDriver::DeviceCaps::FillRect> FillRectImpl;
		typedef FrameRectImpl<MyType, DeviceDriver, DeviceDriver::DeviceCaps::FrameRect> FrameRectImpl;
		typedef RectangleImpl<MyType, DeviceDriver, DeviceDriver::DeviceCaps::Rectangle> RectangleImpl;
		// Реализации дружат с контекстом, для доступа к его защищенным полям
		friend SetPixelImpl;
		friend MoveToImpl;
		friend LineToImpl;
		friend FillRectImpl;
		friend FrameRectImpl;
		friend RectangleImpl;
		
		// Поддерживаемый контекстом набор шрифтов
		enum FontId
		{
			FontTerminal5x12,
			FontTerminal10x17,
			FontArial12,			// FontArial16x16
			FontArial10,			// FontArial13x16
			FontArial8				// FontArial10x14
		};
	protected:
		int _curX;							// текущая горизонтальная координата курсора
		int _curY;							// текущая вертикальная коррдената курсора
		int _curPenColor;					// цвет текущего пера
		int _curBrushColor;					// цвет текущей кисти
		FontBaseType *_curFont;				// текущий шрифт
		
		// Экземпляры шрифтов поддерживаемых контекстом
		FontBaseType _nullFont;				// пустов шрифт (используется как заглушка при ошибках)
		// Шрифты формата GLCD
		FontGlcdType _fontTerminal5x12;	
		FontGlcdType _fontTerminal10x17;
		FontGlcdType _fontArial16x16;
		FontGlcdType _fontArial13x16;
		FontGlcdType _fontArial10x14;
	public:
		DeviceContext() :
			_fontTerminal5x12(FontGlcdType::Terminal5x12),
			_fontTerminal10x17(FontGlcdType::Terminal10x17),
			_fontArial16x16(FontGlcdType::Arial16x16, FontGlcdType::Arial15x16Cyr),
			_fontArial13x16(FontGlcdType::Arial13x16, FontGlcdType::Arial12x16Cyr),
			_fontArial10x14(FontGlcdType::Arial10x14, FontGlcdType::Arial11x13Cyr)
		{
			_curX = 0;
			_curY = 0;
			_curPenColor = 0xFFFFFF;
			_curBrushColor = 0x000000;
			_curFont = &_fontTerminal5x12;
		}

		// Получение значения цвета из целочисленных значений RGB
		int GetColorFromIntRgb(int r, int g, int b)
		{
			return DeviceDriver::GetColorFromIntRgb(r, g, b);
		}

		// Получение значения цвета из вещественных значений RGB
		int GetColorFromFloatRgb(float r, float g, float b)
		{
			return DeviceDriver::GetColorFromFloatRgb(r, g, b);
		}

		// Задать текущий цвет пера
		void SetPenColor(int color)
		{
			_curPenColor = color;
		}
		// Задать текущий цвет кисти
		void SetBrushColor(int color)
		{
			_curBrushColor = color;
		}
		// Задать текущий шрифт
		void SetFont(FontId font)
		{
			switch(font)
			{
			case FontTerminal5x12:
				_curFont = &_fontTerminal5x12;
				break;
			case FontTerminal10x17:
				_curFont = &_fontTerminal10x17;
				break;
			case FontArial12:
				_curFont = &_fontArial16x16;
				break;
			case FontArial10:
				_curFont = &_fontArial13x16;
				break;
			case FontArial8:
				_curFont = &_fontArial10x14;
				break;
			default:
				_curFont = &_nullFont;
				break;
			}
		}
		// Нарисовать точку
		void SetPixel(int x, int y, int color)
		{
			SetPixelImpl::Execute(this, x, y, color);
		}
		// Переместить курсор в заданную позицию
		void MoveTo(int x, int y)
		{
			MoveToImpl::Execute(this, x, y);
		}
		// Нарисовать линию текущим пером из текущей позиции в заданную. Текущая позиция изменяется на заданную.
		void LineTo(int x, int y)
		{
			LineToImpl::Execute(this, x, y);
		}
		// Нарисовать прямоугольник без рамки закрашенный текущей кистью
		void FillRect(int left, int top, int right, int bottom)
		{
			FillRectImpl::Execute(this, left, top, right, bottom);
		}
		// Нарисовать рамку прямоугольника текущим пером
		void FrameRect(int left, int top, int right, int bottom)
		{
			FrameRectImpl::Execute(this, left, top, right, bottom);
		}
		// Нарисовать прямоугольник с рамкой (рамка рисуется текущим пером, закраска ведется текущеё кистью)
		void Rectangle(int left, int top, int right, int bottom)
		{
			RectangleImpl::Execute(this, left, top, right, bottom);
		}
		// Вывести строку.
		// left - координата x верхнего левого угла первого символа строки
		// top - координата y верхнего левого угла первого символа строки
		// str - строка
		// strLen - длина строки
		// Символы рисуются текущим шрифтом, текущим пером.
		// Высота символов определяется шрифотом.
		// Символы могут иметь переменную ширину в зависимости от шрифта.
		// Интервал между символами - одна точка.
		void TextOut(int left, int top, void *str, int strLen)
		{
			int x = left;
			int y = top;
			for (int i = 0; i < strLen; i++)
			{
				x += _curFont->DrawChar(this, (unsigned int)((unsigned char *)str)[i], x, y, _curPenColor);
				x += 1;
			}
		}

		// Получить отображаемый размер строки
		// str - строка
		// strLen - длина строки
		// width - возвращаемая ширина строки
		// heigth - возвращаемая высота строки
		void GetStringSize(void *str, int strLen, int &width, int &heigth)
		{
			width = 0;
			heigth = 0;
			for (int i = 0; i < strLen; i++)
			{
				int w = 0;
				int h = 0;
				_curFont->GetCharSize(this, (unsigned int)((unsigned char *)str)[i], w, h);
				width += w;
				width += 1; // межсимвольный интервал
				heigth = h > heigth ? h : heigth;
			}
		}
		
		// Вывести изображение
		void DrawBitmap(BmpReader *image, int left, int top)
		{
			if (image == 0)
			{
				return;
			}

			if (image->Valid())
			{
				for (int y = 0; y < image->Height(); y++)
				{
					for (int x = 0; x < image->Width(); x++)
					{
						unsigned char r;
						unsigned char g;
						unsigned char b;
						if (image->GetPixel(x, y, r, g, b))
						{
							SetPixel(x + left, y + top, GetColorFromIntRgb(r, g, b));
						}
					}
				}
			}
		}

		// Вывести изображение с прозрачностью (указывается цвет прозрачных пикселей)
		void DrawBitmapTransparent(BmpReader *image, int left, int top, unsigned char rTranparent, unsigned char gTranparent, unsigned char bTranparent)
		{
			if (image == 0)
			{
				return;
			}

			if (image->Valid())
			{
				for (int y = 0; y < image->Height(); y++)
				{
					for (int x = 0; x < image->Width(); x++)
					{
						unsigned char r;
						unsigned char g;
						unsigned char b;
						if (image->GetPixel(x, y, r, g, b))
						{
							if (r != rTranparent || g != gTranparent || b != bTranparent)
							{
								SetPixel(x + left, y + top, GetColorFromIntRgb(r, g, b));
							}
						}
					}
				}
			}
		}

		// Вывести изображение с прозрачностью (цвет прозрачных пикселей определяется по указанному пикселю изображения)
		void DrawBitmapTransparent(BmpReader *image, int left, int top, int xTransparent, int yTransparent)
		{
			if (image == 0)
			{
				return;
			}

			if (image->Valid())
			{
				unsigned char rTranparent;
				unsigned char gTranparent;
				unsigned char bTranparent;

				if (image->GetPixel(xTransparent, yTransparent, rTranparent, gTranparent, bTranparent))
				{
					for (int y = 0; y < image->Height(); y++)
					{
						for (int x = 0; x < image->Width(); x++)
						{
							unsigned char r;
							unsigned char g;
							unsigned char b;
							if (image->GetPixel(x, y, r, g, b))
							{
								if (r != rTranparent || g != gTranparent || b != bTranparent)
								{
									SetPixel(x + left, y + top, GetColorFromIntRgb(r, g, b));
								}
							}
						}
					}
				}
			}
		}

		// Вывести изображение с прозрачностью (цвет прозрачных пикселей определяется по верхне левому углу изображения)
		void DrawBitmapTransparent(BmpReader *image, int left, int top)
		{
			DrawBitmapTransparent(image, left, top, 0, 0);
		}
	};
	
	// Реализация SetPixelImpl через контекст
	template<class DeviceContext, class DeviceDriver>
	class SetPixelImpl<DeviceContext, DeviceDriver, false>
	{
	public:
		static inline void Execute(DeviceContext *dc, int x, int y, int color)
		{
			// ничего не увидим, драйвер даже пиксели рисовать не умеет :(
		}
	};
	
	// Реализация SetPixelImpl через драйвер
	template<class DeviceContext, class DeviceDriver>
	class SetPixelImpl<DeviceContext, DeviceDriver, true>
	{
	public:
		static inline void Execute(DeviceContext *dc, int x, int y, int color)
		{
			DeviceDriver::SetPixel(x, y, color);
		}
	};
	
	// Реализация MoveToImpl через контекст
	template<class DeviceContext, class DeviceDriver>
	class MoveToImpl<DeviceContext, DeviceDriver, false>
	{
	
	public:
		static inline void Execute(DeviceContext *dc, int x, int y)
		{
			dc->_curX = x;
			dc->_curY = y;
		}
	};
	
	// Реализация MoveToImpl через драйвер
	template<class DeviceContext, class DeviceDriver>
	class MoveToImpl<DeviceContext, DeviceDriver, true>
	{
	public:
		static inline void Execute(DeviceContext *dc, int x, int y)
		{
			DeviceDriver::MoveTo(x, y);
		}
	};
	
	// Реализация LineToImpl через контекст
	template<class DeviceContext, class DeviceDriver>
	class LineToImpl<DeviceContext, DeviceDriver, false>
	{
	public:
		static inline void Execute(DeviceContext *dc, int x, int y)
		{
			int x1 = dc->_curX;
			int y1 = dc->_curY;
			int x2 = x;
			int y2 = y;
			const int deltaX = abs(x2 - x1);
			const int deltaY = abs(y2 - y1);
			const int signX = x1 < x2 ? 1 : -1;
			const int signY = y1 < y2 ? 1 : -1;
			int error = deltaX - deltaY;
			while(x1 != x2 || y1 != y2) {
				dc->SetPixel(x1, y1, dc->_curPenColor);
				const int error2 = error * 2;
				//
				if(error2 > -deltaY) {
					error -= deltaY;
					x1 += signX;
				}
				if(error2 < deltaX) {
					error += deltaX;
					y1 += signY;
				}
			}

			dc->SetPixel(x2, y2, dc->_curPenColor);
			
			dc->_curX = x;
			dc->_curY = y;
		}
	};
	
	// Реализация LineToImpl через драйвер
	template<class DeviceContext, class DeviceDriver>
	class LineToImpl<DeviceContext, DeviceDriver, true>
	{
	public:
		static inline void Execute(DeviceContext *dc, int x, int y)
		{
			DeviceDriver::LineTo(x, y);
		}
	};

	// Реализация FillRectImpl через контекст
	template<class DeviceContext, class DeviceDriver>
	class FillRectImpl<DeviceContext, DeviceDriver, false>
	{

	public:
		static inline void Execute(DeviceContext *dc, int left, int top, int right, int bottom)
		{
			if (left > right)
			{
				int tmp = right;
				right = left;
				left = tmp;
			}

			if (top > bottom)
			{
				int tmp = bottom;
				bottom = top;
				top = tmp;
			}

			for (int y = top; y <= bottom; y++)
			{
				for (int x = left; x <= right; x++)
				{
					dc->SetPixel(x, y, dc->_curBrushColor);
				}
			}
		}
	};

	// Реализация FillRectImpl через драйвер
	template<class DeviceContext, class DeviceDriver>
	class FillRectImpl<DeviceContext, DeviceDriver, true>
	{
	public:
		static inline void Execute(DeviceContext *dc, int left, int top, int right, int bottom)
		{
			DeviceDriver::FillRect(left, top, right, bottom);
		}
	};

	// Реализация FrameRectImpl через контекст
	template<class DeviceContext, class DeviceDriver>
	class FrameRectImpl<DeviceContext, DeviceDriver, false>
	{

	public:
		static inline void Execute(DeviceContext *dc, int left, int top, int right, int bottom)
		{
			if (left > right)
			{
				int tmp = right;
				right = left;
				left = tmp;
			}

			if (top > bottom)
			{
				int tmp = bottom;
				bottom = top;
				top = tmp;
			}

			dc->MoveTo(left, top);
			dc->LineTo(right, top);
			dc->LineTo(right, bottom);
			dc->LineTo(left, bottom);
			dc->LineTo(left, top);
		}
	};

	// Реализация FrameRectImpl через драйвер
	template<class DeviceContext, class DeviceDriver>
	class FrameRectImpl<DeviceContext, DeviceDriver, true>
	{
	public:
		static inline void Execute(DeviceContext *dc, int left, int top, int right, int bottom)
		{
			DeviceDriver::FrameRect(left, top, right, bottom);
		}
	};

	// Реализация RectangleImpl через контекст
	template<class DeviceContext, class DeviceDriver>
	class RectangleImpl<DeviceContext, DeviceDriver, false>
	{

	public:
		static inline void Execute(DeviceContext *dc, int left, int top, int right, int bottom)
		{
			dc->FrameRect(left, top, right, bottom);
			dc->FillRect(left + 1, top + 1, right - 1, bottom - 1);
		}
	};

	// Реализация RectangleImpl через драйвер
	template<class DeviceContext, class DeviceDriver>
	class RectangleImpl<DeviceContext, DeviceDriver, true>
	{
	public:
		static inline void Execute(DeviceContext *dc, int left, int top, int right, int bottom)
		{
			DeviceDriver::Rectangle(left, top, right, bottom);
		}
	};
}
}

#endif