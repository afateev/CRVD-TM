///////////////////////////////////////////////////////////////////////////////
//	�������� ������������ ����������
//	��������� ������ ���������
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
	// ���������� SetPixelImpl
	// � ����������� �� driverImplemented, ����������� ��������� ��� ����������� ���������� ���������� 
	template<class DeviceContext, class DeviceDriver, bool driverImplemented>
	class SetPixelImpl
	{
	};
	
	// ���������� MoveToImpl
	// � ����������� �� driverImplemented, ����������� ��������� ��� ����������� ���������� ����������
	template<class DeviceContext, class DeviceDriver, bool driverImplemented>
	class MoveToImpl
	{
	};
	
	// ���������� LineToImpl
	// � ����������� �� driverImplemented, ����������� ��������� ��� ����������� ���������� ����������
	template<class DeviceContext, class DeviceDriver, bool driverImplemented>
	class LineToImpl
	{
	};

	// ���������� FillRectImpl
	// � ����������� �� driverImplemented, ����������� ��������� ��� ����������� ���������� ����������
	template<class DeviceContext, class DeviceDriver, bool driverImplemented>
	class FillRectImpl
	{
	};

	// ���������� FrameRectImpl
	// � ����������� �� driverImplemented, ����������� ��������� ��� ����������� ���������� ����������
	template<class DeviceContext, class DeviceDriver, bool driverImplemented>
	class FrameRectImpl
	{
	};

	// ���������� RectangleImpl
	// � ����������� �� driverImplemented, ����������� ��������� ��� ����������� ���������� ����������
	template<class DeviceContext, class DeviceDriver, bool driverImplemented>
	class RectangleImpl
	{
	};
		
	// �������� ����������
	template<class DeviceDriver>
	class DeviceContext
	{
	public:
		typedef DeviceContext<DeviceDriver> MyType;
		typedef Font<MyType> FontBaseType;
		typedef FontGlcd<MyType> FontGlcdType;
		// ����� ���������� � ����������� �� ������������ ��������
		typedef SetPixelImpl<MyType, DeviceDriver, DeviceDriver::DeviceCaps::SetPixel> SetPixelImpl;
		typedef MoveToImpl<MyType, DeviceDriver, DeviceDriver::DeviceCaps::MoveTo> MoveToImpl;
		typedef LineToImpl<MyType, DeviceDriver, DeviceDriver::DeviceCaps::LineTo> LineToImpl;
		typedef FillRectImpl<MyType, DeviceDriver, DeviceDriver::DeviceCaps::FillRect> FillRectImpl;
		typedef FrameRectImpl<MyType, DeviceDriver, DeviceDriver::DeviceCaps::FrameRect> FrameRectImpl;
		typedef RectangleImpl<MyType, DeviceDriver, DeviceDriver::DeviceCaps::Rectangle> RectangleImpl;
		// ���������� ������ � ����������, ��� ������� � ��� ���������� �����
		friend SetPixelImpl;
		friend MoveToImpl;
		friend LineToImpl;
		friend FillRectImpl;
		friend FrameRectImpl;
		friend RectangleImpl;
		
		// �������������� ���������� ����� �������
		enum FontId
		{
			FontTerminal5x12,
			FontTerminal10x17,
			FontArial12,			// FontArial16x16
			FontArial10,			// FontArial13x16
			FontArial8				// FontArial10x14
		};
	protected:
		int _curX;							// ������� �������������� ���������� �������
		int _curY;							// ������� ������������ ���������� �������
		int _curPenColor;					// ���� �������� ����
		int _curBrushColor;					// ���� ������� �����
		FontBaseType *_curFont;				// ������� �����
		
		// ���������� ������� �������������� ����������
		FontBaseType _nullFont;				// ������ ����� (������������ ��� �������� ��� �������)
		// ������ ������� GLCD
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

		// ��������� �������� ����� �� ������������� �������� RGB
		int GetColorFromIntRgb(int r, int g, int b)
		{
			return DeviceDriver::GetColorFromIntRgb(r, g, b);
		}

		// ��������� �������� ����� �� ������������ �������� RGB
		int GetColorFromFloatRgb(float r, float g, float b)
		{
			return DeviceDriver::GetColorFromFloatRgb(r, g, b);
		}

		// ������ ������� ���� ����
		void SetPenColor(int color)
		{
			_curPenColor = color;
		}
		// ������ ������� ���� �����
		void SetBrushColor(int color)
		{
			_curBrushColor = color;
		}
		// ������ ������� �����
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
		// ���������� �����
		void SetPixel(int x, int y, int color)
		{
			SetPixelImpl::Execute(this, x, y, color);
		}
		// ����������� ������ � �������� �������
		void MoveTo(int x, int y)
		{
			MoveToImpl::Execute(this, x, y);
		}
		// ���������� ����� ������� ����� �� ������� ������� � ��������. ������� ������� ���������� �� ��������.
		void LineTo(int x, int y)
		{
			LineToImpl::Execute(this, x, y);
		}
		// ���������� ������������� ��� ����� ����������� ������� ������
		void FillRect(int left, int top, int right, int bottom)
		{
			FillRectImpl::Execute(this, left, top, right, bottom);
		}
		// ���������� ����� �������������� ������� �����
		void FrameRect(int left, int top, int right, int bottom)
		{
			FrameRectImpl::Execute(this, left, top, right, bottom);
		}
		// ���������� ������������� � ������ (����� �������� ������� �����, �������� ������� ������ ������)
		void Rectangle(int left, int top, int right, int bottom)
		{
			RectangleImpl::Execute(this, left, top, right, bottom);
		}
		// ������� ������.
		// left - ���������� x �������� ������ ���� ������� ������� ������
		// top - ���������� y �������� ������ ���� ������� ������� ������
		// str - ������
		// strLen - ����� ������
		// ������� �������� ������� �������, ������� �����.
		// ������ �������� ������������ ��������.
		// ������� ����� ����� ���������� ������ � ����������� �� ������.
		// �������� ����� ��������� - ���� �����.
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

		// �������� ������������ ������ ������
		// str - ������
		// strLen - ����� ������
		// width - ������������ ������ ������
		// heigth - ������������ ������ ������
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
				width += 1; // ������������� ��������
				heigth = h > heigth ? h : heigth;
			}
		}
		
		// ������� �����������
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

		// ������� ����������� � ������������� (����������� ���� ���������� ��������)
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

		// ������� ����������� � ������������� (���� ���������� �������� ������������ �� ���������� ������� �����������)
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

		// ������� ����������� � ������������� (���� ���������� �������� ������������ �� ������ ������ ���� �����������)
		void DrawBitmapTransparent(BmpReader *image, int left, int top)
		{
			DrawBitmapTransparent(image, left, top, 0, 0);
		}
	};
	
	// ���������� SetPixelImpl ����� ��������
	template<class DeviceContext, class DeviceDriver>
	class SetPixelImpl<DeviceContext, DeviceDriver, false>
	{
	public:
		static inline void Execute(DeviceContext *dc, int x, int y, int color)
		{
			// ������ �� ������, ������� ���� ������� �������� �� ����� :(
		}
	};
	
	// ���������� SetPixelImpl ����� �������
	template<class DeviceContext, class DeviceDriver>
	class SetPixelImpl<DeviceContext, DeviceDriver, true>
	{
	public:
		static inline void Execute(DeviceContext *dc, int x, int y, int color)
		{
			DeviceDriver::SetPixel(x, y, color);
		}
	};
	
	// ���������� MoveToImpl ����� ��������
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
	
	// ���������� MoveToImpl ����� �������
	template<class DeviceContext, class DeviceDriver>
	class MoveToImpl<DeviceContext, DeviceDriver, true>
	{
	public:
		static inline void Execute(DeviceContext *dc, int x, int y)
		{
			DeviceDriver::MoveTo(x, y);
		}
	};
	
	// ���������� LineToImpl ����� ��������
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
	
	// ���������� LineToImpl ����� �������
	template<class DeviceContext, class DeviceDriver>
	class LineToImpl<DeviceContext, DeviceDriver, true>
	{
	public:
		static inline void Execute(DeviceContext *dc, int x, int y)
		{
			DeviceDriver::LineTo(x, y);
		}
	};

	// ���������� FillRectImpl ����� ��������
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

	// ���������� FillRectImpl ����� �������
	template<class DeviceContext, class DeviceDriver>
	class FillRectImpl<DeviceContext, DeviceDriver, true>
	{
	public:
		static inline void Execute(DeviceContext *dc, int left, int top, int right, int bottom)
		{
			DeviceDriver::FillRect(left, top, right, bottom);
		}
	};

	// ���������� FrameRectImpl ����� ��������
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

	// ���������� FrameRectImpl ����� �������
	template<class DeviceContext, class DeviceDriver>
	class FrameRectImpl<DeviceContext, DeviceDriver, true>
	{
	public:
		static inline void Execute(DeviceContext *dc, int left, int top, int right, int bottom)
		{
			DeviceDriver::FrameRect(left, top, right, bottom);
		}
	};

	// ���������� RectangleImpl ����� ��������
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

	// ���������� RectangleImpl ����� �������
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