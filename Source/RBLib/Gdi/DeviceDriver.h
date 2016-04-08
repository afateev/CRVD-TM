///////////////////////////////////////////////////////////////////////////////
//	Интерфейс драйвера графического устройтсва
//	Каждый драйвер должен риализовывать этот интерфейс хотя бы частитчно
//
//  Обязательно определить структуру DeviceCaps (переопределить, если драйвер устройства наследуется от этого класса)
//	Она определяет набор возможностей драйвера
//	Если какая то возможность не реализуется драйвером, то контекст устройтсва эмулирует эту возможность
//
//	Каждый драйвер должен реализовывать хотя бы SetPixel, чтобы что-то нарисовать
//	
///////////////////////////////////////////////////////////////////////////////

#ifndef GDI_DEVICEDRIVER_H
#define GDI_DEVICEDRIVER_H

namespace Rblib
{
namespace Gdi
{
	class DeviceDriver
	{
	public:
		// Набор возможностей драйвера
		// Например,
		//    если LineTo == true,
		//    то драйвер реализует метод LineTo,
		//    иначе метод LineTo не реализуется и может отсутствовать
		struct DeviceCaps
		{
			static const bool SetPixel = false;
			static const bool MoveTo = false;
			static const bool LineTo = false;
			static const bool FillRect = false;
			static const bool FrameRect = false;
			static const bool Rectangle = false;
		};
		
		// Получение значения цвета из целочисленных значений RGB
		static int GetColorFromIntRgb(int r, int g, int b);
		// Получение значения цвета из вещественных значений RGB
		static int GetColorFromFloatRgb(float r, float g, float b);
		
		static void SetPixel(int x, int y, int color);
		static void MoveTo(int x, int y);
		static void LineTo(int x, int y);
	};
}
}

#endif